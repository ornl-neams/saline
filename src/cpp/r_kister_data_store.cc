#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "saline_bug.hh"
#ifdef SALINE_USE_HDF5
#include <hdf5.h>
#endif

#include "data_store.hh"
#include "default_data_store.hh"
#include "r_kister_data_store.hh"
#include "utils.hh"

// TODO all the enthalpy functions. They rely on setting up enthalpy tables.
// This is done for all the end members, but its not clear that carries much
// meaning for compositions of end members. We certainly shouldn't set up
// enthalpy tables for every possible combination of end members and we don't
// even know which tables would be used or when.

namespace saline {
//@{
//! Types
using Id = std::size_t;
using Name = std::string;
using Vec_Id = std::vector<Id>;
using Vec_Name = std::vector<Name>;
using Vec_Mole = std::vector<double>;
//@}

// Boltzmann's constant 1.380649 × 10-23 m2 kg s-2 K-1
static constexpr double BOLTZMANN_CONSTANT = 1.380649e-23;
// Avogadro's Number
static constexpr double AVOGADROS_NUMBER = 6.02214076e23;
//----------------------------------------------------------------------------//
/*!
 * \brief constructs the Redlich-Kister data store extension
 */
R_Kister_Data_Store::R_Kister_Data_Store() {}

//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void R_Kister_Data_Store::load(const std::string &fPath) {
  std::ifstream inFile(fPath, std::ios::binary);
  d.load(fPath);
  if (!inFile) {
    std::cerr << "Failed to open file: " << fPath << " !\n";
    return;
  }
  if (utils::is_hdf5_file(fPath)) {
#ifdef SALINE_USE_HDF5
    // Open the HDF5 file
    hid_t file = H5Fopen(fPath.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    from_h5(file);
    // Close the file
    H5Fclose(file);
#else
    throw std::runtime_error("Saline is not configured to use HDF5!");
#endif
  } else {
    from_json(inFile);
  }
  auto d_keys = d.getSaltKeys();
  std::copy_if(
      d_keys.begin(), d_keys.end(), std::back_inserter(m_keys),
      [](const std::string &s) { return s.find('-') == std::string::npos; });
}

//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void R_Kister_Data_Store::load(const std::string &rkDens,
                               const std::string &rkVisc,
                               const std::string &dfPath) {
  // Set up a default data store
  d = Default_Data_Store();
  std::ifstream inFile(dfPath.data());
  if (!inFile.is_open()) {
    throw std::runtime_error("Falied to open input data file.");
  }

  std::string rk_dens = rkDens.data();
  std::ifstream rkRhoFile(rk_dens.data());
  if (!rkRhoFile.is_open()) {
    throw std::runtime_error("Falied to open input RK density data file.");
  }
  std::string rk_visc = rkVisc.data();
  std::ifstream rkMuFile(rk_visc.data());
  if (!rkMuFile.is_open()) {
    throw std::runtime_error("Falied to open input RK viscosity data file.");
  }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  load(rkRhoFile, rkMuFile, inFile);
#pragma GCC diagnostic pop
  auto d_keys = d.getSaltKeys();
  std::copy_if(
      d_keys.begin(), d_keys.end(), std::back_inserter(m_keys),
      [](const std::string &s) { return s.find('-') == std::string::npos; });
}
//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void R_Kister_Data_Store::load(const std::string &rkDataPath,
                               const std::string &dfPath) {
  // Set up a default data store
  d = Default_Data_Store();
  std::ifstream inFile(dfPath.data());
  if (!inFile.is_open()) {
    throw std::runtime_error("Falied to open input data file.");
  }

  std::string rk_dens =
      rkDataPath + "/Molten_Salt_Thermophysical_Properties_Dens.csv";
  std::ifstream rkRhoFile(rk_dens.data());
  if (!rkRhoFile.is_open()) {
    throw std::runtime_error("Falied to open input RK density data file.");
  }
  std::string rk_visc =
      rkDataPath + "/Molten_Salt_Thermophysical_Properties_Visc.csv";
  std::ifstream rkMuFile(rk_visc.data());
  if (!rkMuFile.is_open()) {
    throw std::runtime_error("Falied to open input RK viscosity data file.");
  }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  load(rkRhoFile, rkMuFile, inFile);
#pragma GCC diagnostic pop
  auto d_keys = d.getSaltKeys();
  std::copy_if(
      d_keys.begin(), d_keys.end(), std::back_inserter(m_keys),
      [](const std::string &s) { return s.find('-') == std::string::npos; });
}
//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data Redlich-Kister parameters into data store
 */
void R_Kister_Data_Store::load(std::istream &rkRhoFile, std::istream &rkMuFile,
                               std::istream &inFile) {
  // set up default store
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  d.load(inFile);
#pragma GCC diagnostic pop
  // Set up space for the mixing models
  Vec_Name keys = d.getSaltKeys();

  m_rho.resize(d.size(),
               std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  m_mu.resize(d.size(),
              std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  ////TODO no input for these, implemented only for downstream process
  m_cp.resize(d.size(),
              std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  m_k.resize(d.size(),
             std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));

  // Jaunt through lines until we find the Redlich-Kister parameters
  std::string line;
  // this is  a comment line
  std::getline(rkRhoFile, line);

  // Read the input data. TODO currently only uses density
  while (std::getline(rkRhoFile, line)) {
    auto tokens = utils::split(",", line);
    for (size_t i = 0; i < tokens.size(); ++i) {
      utils::trim(tokens[i]);
    }

    // Sort components and adjust value if required
    // L1_a, L1_b, L2_a, L2_b, L3_a, L3_b, L4_a, L4_b
    size_t id_a;
    size_t id_b;
    double sortFactor;
    if (tokens[0] < tokens[1]) {
      id_a = d.names_to_id({tokens[0]});
      id_b = d.names_to_id({tokens[1]});
      sortFactor = 1.0;
    } else {
      id_a = d.names_to_id({tokens[1]});
      id_b = d.names_to_id({tokens[0]});
      sortFactor = -1.0;
    }

    // component a
    m_rho[id_a][id_b].a_n.resize(4);
    m_rho[id_a][id_b].a_n[0] = std::stod(tokens[2]);
    m_rho[id_a][id_b].a_n[1] = sortFactor * std::stod(tokens[4]);
    m_rho[id_a][id_b].a_n[2] = std::stod(tokens[6]);
    // componet b
    m_rho[id_a][id_b].b_n.resize(4);
    m_rho[id_a][id_b].b_n[0] = std::stod(tokens[3]);
    m_rho[id_a][id_b].b_n[1] = sortFactor * std::stod(tokens[5]);
    m_rho[id_a][id_b].b_n[2] = std::stod(tokens[7]);

    // T_min, T_max
    m_rho[id_a][id_b].t_min = std::stod(tokens[8]);
    m_rho[id_a][id_b].t_max = std::stod(tokens[9]);

    // algorithm
    // reference
    std::string ref;
    for (size_t i = 10; i < tokens.size(); ++i) {
      ref += tokens[i];
    }
    m_rho[id_a][id_b].reference = ref;
  }

  std::getline(rkMuFile, line);

  // Read the input data. TODO currently only uses density
  while (std::getline(rkMuFile, line)) {
    auto tokens = utils::split(",", line);
    for (size_t i = 0; i < tokens.size(); ++i) {
      utils::trim(tokens[i]);
    }

    // Sort components and adjust value if required
    // L1_a, L1_b, L2_a, L2_b, L3_a, L3_b, L4_a, L4_b
    size_t id_a;
    size_t id_b;
    double sortFactor;
    if (tokens[0] < tokens[1]) {
      id_a = d.names_to_id({tokens[0]});
      id_b = d.names_to_id({tokens[1]});
      sortFactor = 1.0;
    } else {
      id_a = d.names_to_id({tokens[1]});
      id_b = d.names_to_id({tokens[0]});
      sortFactor = -1.0;
    }

    // component a
    m_mu[id_a][id_b].a_n.resize(4);
    m_mu[id_a][id_b].a_n[0] = std::stod(tokens[2]);
    m_mu[id_a][id_b].a_n[1] = sortFactor * std::stod(tokens[5]);
    m_mu[id_a][id_b].a_n[2] = std::stod(tokens[8]);
    // componet b
    m_mu[id_a][id_b].b_n.resize(4);
    m_mu[id_a][id_b].b_n[0] = std::stod(tokens[3]);
    m_mu[id_a][id_b].b_n[1] = sortFactor * std::stod(tokens[6]);
    m_mu[id_a][id_b].b_n[2] = std::stod(tokens[9]);
    // componet c
    m_mu[id_a][id_b].c_n.resize(4);
    m_mu[id_a][id_b].c_n[0] = std::stod(tokens[4]);
    m_mu[id_a][id_b].c_n[1] = sortFactor * std::stod(tokens[7]);
    m_mu[id_a][id_b].c_n[2] = std::stod(tokens[10]);

    // T_min, T_max
    m_mu[id_a][id_b].t_min = std::stod(tokens[11]);
    m_mu[id_a][id_b].t_max = std::stod(tokens[12]);

    // algorithm
    // reference
    m_mu[id_a][id_b].reference = tokens[13];
  }
}
//----------------------------------------------------------------------------//
/*!
 * \brief finds and sets the required information for modeling a requested salt
 */
Data_Store::View R_Kister_Data_Store::setView(const Vec_Name &names,
                                              const Vec_Mole &mole_percents) {
  // Clear the old end members
  end_members.clear();

  // Set up a view or returning useful data
  Data_Store::View v;

  end_members.resize(names.size());
  endMem_moleFracs = mole_percents;
  salt_name = names;

  // For the functioning of this data store...views of end_members are stored
  for (size_t i = 0; i < names.size(); ++i) {
    auto iname = names[i];
    if (d.valid(iname))
      end_members[i] = (d.setView({names[i]}, {1.0}));
  }

  if (std::all_of(end_members.begin(), end_members.end(),
                  [](Data_Store::View v) { return !v.nullView(); })) {
    v = view(0);
  }
  return v;
}

//----------------------------------------------------------------------------//
/*!
 * \brief returns the names of the compound currently being investigated
 */
Vec_Name R_Kister_Data_Store::names(Id /* id */) const {
  Vec_Name comp_names;
  return comp_names;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on
 * temperature
 */
double R_Kister_Data_Store::cp(Id /* id */, Id /* data_id */,
                               double temperature,
                               double /* pressure */) const {
  // Ideal mixing
  double cp_ideal = 0.0;
  for (size_t i = 0; i < end_members.size(); ++i) {
    cp_ideal += end_members[i].cp(temperature) * endMem_moleFracs[i];
  }

  // Excess
  double cp_excess = 0.0;
  for (size_t j = 0; j < end_members.size() - 1; ++j) {
    int j_id = end_members[j].id;
    for (size_t i = j + 1; i < end_members.size(); ++i) {
      int i_id = end_members[i].id;
      RK_Polynomial excess_calc = m_cp[j_id][i_id];
      cp_excess += excess_calc.getRK_solution(endMem_moleFracs[j],
                                              endMem_moleFracs[i], temperature);
    }
  }
  return cp_ideal + cp_excess;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::cp_h(Id /* id */, Id /* data_id */,
                                 double /* enthalpy */,
                                 double /* pressure */) const {
  // TODO waiting on enthalpy calculation flow
  double cp = 0.0;
  return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on temperature
 */
double R_Kister_Data_Store::mu(Id /* id */, Id /* data_id */,
                               double temperature,
                               double /* pressure */) const {
  // Ideal mixing
  double mu_ideal = 0.0;
  for (size_t i = 0; i < end_members.size(); ++i) {
    mu_ideal += std::log(end_members[i].mu(temperature)) * endMem_moleFracs[i];
  }

  // Excess
  double mu_excess = 0.0;
  for (size_t j = 0; j < end_members.size() - 1; ++j) {
    int j_id = end_members[j].id;
    for (size_t i = j + 1; i < end_members.size(); ++i) {
      int i_id = end_members[i].id;
      RK_Polynomial excess_calc = m_mu[j_id][i_id];
      mu_excess += excess_calc.getRK_viscsolution(
          endMem_moleFracs[j], endMem_moleFracs[i], temperature);
    }
  }
  return std::exp(mu_ideal + mu_excess);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::mu_h(Id /* id */, Id /* data_id */,
                                 double /* enthalpy */,
                                 double /* pressure */) const {
  // TODO waiting on enthalpy calculation flow
  double cp = 0.0;
  return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on
 * temperature
 */
double R_Kister_Data_Store::k(Id id, Id data_id, double temperature,
                              double /* pressure */) const {
  // TODO DBC insist temperature is greater than t_melt

  // Melt temperature of the mixture
  double t_melt = melt(id, data_id);
  saline_insist(t_melt > 0.0,
                "Melting temperature is unknown for this composition");

  // Mixture speed of sound (m * s^-1)
  double mix_c0 = speedOfSound(id, data_id, t_melt);
  saline_insist(mix_c0 > 0.0, "speed of sound is unknown for this composition");

  // Mixture heat capacity (J k^-1 mol^-1)
  double mix_cp = cp(id, data_id, t_melt);
  saline_insist(mix_cp > 0.0, "heat capacity is unknown for this composition");

  // Mixture volume
  double mix_vol = 0.0;
  std::vector<double> vol_fracs(end_members.size());
  for (size_t i = 0; i < end_members.size(); i++) {
    // Uses density and mass of the components
    vol_fracs[i] = end_members[i].molecularWeight() /
                   end_members[i].rho(t_melt) * endMem_moleFracs[i];
    mix_vol += vol_fracs[i];
  }
  // volume fractions of the compoennts
  std::transform(vol_fracs.begin(), vol_fracs.end(), vol_fracs.begin(),
                 [mix_vol](double d) { return d / mix_vol; });

  // Mixutre number density
  // Weighted number of ions in the mixuture
  double n_mix = n_ions(id, data_id);
  // converted to m^-3
  double mix_numdens = AVOGADROS_NUMBER * 1E9 * n_mix / mix_vol;

  // mat Const
  double alpha = 0.0;
  for (size_t i = 0; i < end_members.size(); i++) {
    // thermal expansivity
    alpha += vol_fracs[i] * std::log(end_members[i].rho(t_melt + 1) /
                                     end_members[i].rho(t_melt));
  }

  // Gruneisen parameter (dimensionless)
  // Mixture molar weight (in kg * mol^-1 )
  double mix_mw = molecularWeight(id, data_id) / 1000.0;
  // Mixture material constant (dimensionless)
  double mix_complex = complexation(id, data_id);
  double gamma = mix_mw * alpha * mix_c0 * mix_c0 / mix_cp;

  return mix_complex * BOLTZMANN_CONSTANT * std::pow(mix_numdens, 2.0 / 3.0) *
         mix_c0 *
         (1.0 - (alpha * (gamma + (1.0 / 3.0)) * (temperature - t_melt)));
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::k_h(Id /* id */, Id /* data_id */,
                                double /* enthalpy */,
                                double /* pressure */) const {
  // TODO waiting on enthalpy calculation flow
  double cp = 0.0;
  return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on temperature
 */
double R_Kister_Data_Store::rho(Id /* id */, Id /* data_id */,
                                double temperature,
                                double /* pressure */) const {
  // Only one end member means that density is equal to its density
  if (end_members.size() == 1)
    return end_members[0].rho(temperature);

  // Ideal component
  double m_t = 0.0;
  double v_t = 0.0;
  for (size_t i = 0; i < end_members.size(); ++i) {
    // Component molecular weight contribution
    double endMass = endMem_moleFracs[i] * end_members[i].molecularWeight();
    // Sum of molecular weights contributions
    m_t += endMass;
    // Sum of volume fraction contributions
    v_t += endMass / end_members[i].rho(temperature);
  }
  double rho_ideal = m_t / v_t;

  // Excess component
  double rho_excess = 0.0;
  for (size_t j = 0; j < end_members.size() - 1; ++j) {
    int j_id = end_members[j].id;
    for (size_t i = j + 1; i < end_members.size(); ++i) {
      int i_id = end_members[i].id;
      RK_Polynomial excess_calc = m_rho[j_id][i_id];
      rho_excess += excess_calc.getRK_solution(
          endMem_moleFracs[j], endMem_moleFracs[i], temperature);
    }
  }
  return rho_ideal + rho_excess;
}

int R_Kister_Data_Store::complexation(Id /* id */, Id /* data_id */) const {
  double mix_complex = 0.0;
  for (size_t i = 0; i < end_members.size(); i++) {
    // Also seems slightly unbound
    mix_complex += end_members[i].complexation() * endMem_moleFracs[i];
  }
  return mix_complex;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the speed of sound for the selected compound based on
 * temperature
 */
double R_Kister_Data_Store::n_ions(Id /* id */, Id /* data_id */) const {
  double n_mix = 0;
  for (size_t i = 0; i < end_members.size(); i++) {
    // Also seems slightly unbound
    n_mix += end_members[i].n_ions() * endMem_moleFracs[i];
  }
  return n_mix;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the speed of sound for the selected compound based on
 * temperature
 */
double R_Kister_Data_Store::speedOfSound(Id id, Id data_id,
                                         double temperature) const {
  double mix_c0 = 0.0;
  if (valid_speedOfSound(id, data_id)) {
    for (size_t i = 0; i < end_members.size(); i++) {
      mix_c0 += end_members[i].speedOfSound(temperature) * endMem_moleFracs[i];
    }
  }
  return mix_c0;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension for the selected compound based on
 * temperature
 */
double R_Kister_Data_Store::surfaceTension(Id /* id */, Id /* data_id */,
                                           double /* temperature */,
                                           double /* pressure */) const {
  return -1.0;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::rho_h(Id /* id */, Id /* data_id */,
                                  double /* enthalpy */,
                                  double /* pressure */) const {
  // Ideal mixing
  double h_ideal = 0.0;

  // Excess
  double h_excess = 0.0;
  return h_ideal + h_excess;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension for the selected compound based on
 * enthalpy
 */
double R_Kister_Data_Store::surfaceTension_h(Id /* id */, Id /* data_id */,
                                             double /* enthalpy */,
                                             double /* pressure */) const {
  return -1.0;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the enthalpy for the selected compound based on temperature
 */
// enthalpy
double R_Kister_Data_Store::h_t(Id /* id */, Id /* data_id */,
                                double /* temperature */) const {
  // TODO waiting on enthalpy calculation flow
  double cp = 0.0;
  return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the temperature for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::t_h(Id /* id */, Id /* data_id */,
                                double /* enthalpy */) const {
  // TODO waiting on enthalpy calculation flow
  double cp = 0.0;
  return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the melting temperature of the selected compound
 */
double R_Kister_Data_Store::melt(Id /* id */, Id /* data_id */) const {
  // We do not reliably have melting temperature beyond Ternaries right now
  if (salt_name.size() > 3)
    return 0.0;
  // We cannot reliably estimate melting temperature
  Id salt_id = d.names_to_id(salt_name);
  if (!d.valid(salt_id)) {
    return 0.0;
  }

  // A valid salt has at least one entry
  Id comp_id = d.nearest(salt_id, endMem_moleFracs);
  return d.melt(salt_id, comp_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the boiling temperature of the selected compound
 */
double R_Kister_Data_Store::boil(Id /* id */, Id /* data_id */) const {
  // TODO I am not sure what we are doing here. Deferring until I have an
  // answer
  double cp = 0.0;
  return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief return the number of compounds stored in this data store
 */
std::size_t R_Kister_Data_Store::constituent_count(Id id) const {
  // TODO It isn't immediately clear what this function should return.
  // Is the constituent count the number of end members, binaries, ternaries?
  return id;
}

//----------------------------------------------------------------------------//
/*!
 * \brief returns the ID of the nearest matching compound composition
 */
std::size_t
R_Kister_Data_Store::nearest(Id id, const Vec_Mole & /* mole_percent */) const {
  // TODO This doesn't actually have a meaning at all
  return id;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
double R_Kister_Data_Store::molecularWeight(Id /* id */,
                                            Id /* data_id */) const {
  double molWt = 0.0;
  for (size_t i = 0; i < end_members.size(); ++i) {
    molWt += endMem_moleFracs[i] * end_members[i].molecularWeight();
  }
  return molWt;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
const R_Kister_Data_Store::Vec_Mole &
R_Kister_Data_Store::mole_percent(Id /* id */, Id /* data_id */) const {
  return endMem_moleFracs;
}

//----------------------------------------------------------------------------//
/*!
 * \brief Calculates the solution to a Redlich-Kister polynomial
 */
double R_Kister_Data_Store::RK_Polynomial::getRK_solution(double x, double y,
                                                          double temperature) {
  // Without any fit coefficients the result should be 0.0
  if (a_n.size() < 1)
    return 0.0;

  // Construct the fit terms
  std::vector<double> l_n(a_n.size());
  for (size_t i = 0; i < l_n.size(); ++i) {
    l_n[i] = a_n[i] + (b_n[i] * temperature);
  }

  // difference of weights used to control the fit
  double w_diff = x - y;

  // First term is unweighted
  double summation = l_n[0];
  for (size_t i = 1; i < l_n.size(); ++i) {
    summation += l_n[i] * std::pow(w_diff, i);
  }

  return (x * y) * summation;
}
double
R_Kister_Data_Store::RK_Polynomial::getRK_viscsolution(double x, double y,
                                                       double temperature) {
  // Without any fit coefficients the result should be 0.0
  if (a_n.size() < 1)
    return 0.0;

  // Construct the fit terms
  std::vector<double> l_n(a_n.size());
  for (size_t i = 0; i < l_n.size(); ++i) {
    l_n[i] =
        a_n[i] + (b_n[i] * temperature) + (c_n[i] * temperature * temperature);
  }

  // difference of weights used to control the fit
  double w_diff = x - y;

  // First term is unweighted
  double summation = l_n[0];
  for (size_t i = 1; i < l_n.size(); ++i) {
    summation += l_n[i] * std::pow(w_diff, i);
  }

  return (x * y) * summation;
}

//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool R_Kister_Data_Store::valid_rho(Id /* id */, Id /* data_id */) const {
  return (std::all_of(end_members.begin(), end_members.end(),
                      [](Data_Store::View v) { return v.valid_rho(); }));
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool R_Kister_Data_Store::valid_surfaceTension(Id /* id */,
                                               Id /* data_id */) const {
  return (
      std::all_of(end_members.begin(), end_members.end(),
                  [](Data_Store::View v) { return v.valid_surfaceTension(); }));
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool R_Kister_Data_Store::valid_mu(Id /* id */, Id /* data_id */) const {
  return (std::all_of(end_members.begin(), end_members.end(),
                      [](Data_Store::View v) { return v.valid_mu(); }));
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool R_Kister_Data_Store::valid_k(Id /* id */, Id /* data_id */) const {
  return (std::all_of(end_members.begin(), end_members.end(),
                      [](Data_Store::View v) { return v.valid_k(); }));
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool R_Kister_Data_Store::valid_cp(Id /* id */, Id /* data_id */) const {
  return (std::all_of(end_members.begin(), end_members.end(),
                      [](Data_Store::View v) { return v.valid_cp(); }));
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool R_Kister_Data_Store::valid_speedOfSound(Id /* id */,
                                             Id /* data_id */) const {
  return (
      std::all_of(end_members.begin(), end_members.end(),
                  [](Data_Store::View v) { return v.valid_speedOfSound(); }));
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected
 * compound
 */
std::pair<double, double> R_Kister_Data_Store::rho_rng(Id /* id */,
                                                       Id /* data_id */) const {
  return {0.0, 0.0};
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension experimental range for the selected
 * compound
 */
std::pair<double, double>
R_Kister_Data_Store::surfaceTension_rng(Id /* id */, Id /* data_id */) const {
  return {0.0, 0.0};
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected
 * compound
 */
std::pair<double, double> R_Kister_Data_Store::k_rng(Id /* id */,
                                                     Id /* data_id */) const {
  return {0.0, 0.0};
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity experimental range for the selected compound
 */
std::pair<double, double> R_Kister_Data_Store::mu_rng(Id /* id */,
                                                      Id /* data_id */) const {
  return {0.0, 0.0};
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity experimental range for the selected
 * compound
 */
std::pair<double, double> R_Kister_Data_Store::cp_rng(Id /* id */,
                                                      Id /* data_id */) const {
  return {0.0, 0.0};
}
//----------------------------------------------------------------------------//
/*!
 * \brief Test if a salt is valid
 */
bool R_Kister_Data_Store::valid(Id id) const {
  // It is anticipated that this is only used internally. R_kister_data_store
  // sets this to 0 on successful loading of end members
  return id == 0;
}

bool R_Kister_Data_Store::valid(Vec_Name &names) const {
  for (size_t i = 0; i < names.size(); ++i) {
    if (!d.valid(d.name_to_id(names[i])))
      return false;
  }
  return true;
}

bool R_Kister_Data_Store::valid(Name &name) const {
  return d.valid(d.name_to_id(name));
}

void R_Kister_Data_Store::from_json(std::istream &inFile) {

  nlohmann::json json_in;
  try {
    inFile >> json_in;
  } catch (const nlohmann::json::parse_error &e) {
    std::cerr << "JSON parse error: " << e.what() << '\n';
    return;
  }

  Vec_Name keys = d.getSaltKeys();
  m_rho.resize(d.size(),
               std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  m_mu.resize(d.size(),
              std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));

  ////TODO no input for these, implemented only for downstream process
  m_cp.resize(d.size(),
              std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  m_k.resize(d.size(),
             std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  if (json_in["MSTDBTP"].contains("estimates")) {
    for (auto &[saltname, node] :
         json_in["MSTDBTP"]["estimates"]["RK"]["Density"].items()) {
      Vec_Name names = utils::split("-", saltname);
      size_t j = d.name_to_id(names[0]);
      size_t i = d.name_to_id(names[1]);
      if (node.contains("A"))
        node.at("A").get_to(m_rho[j][i].a_n);
      if (node.contains("B"))
        node.at("B").get_to(m_rho[j][i].b_n);
      if (node.contains("C"))
        node.at("C").get_to(m_rho[j][i].c_n);
      if (node.contains("t_min"))
        node.at("t_min").get_to(m_rho[j][i].t_min);
      if (node.contains("t_max"))
        node.at("t_max").get_to(m_rho[j][i].t_max);
      if (node.contains("reference"))
        node.at("reference").get_to(m_rho[j][i].reference);
    }
    for (auto &[saltname, node] :
         json_in["MSTDBTP"]["estimates"]["RK"]["Viscosity"].items()) {
      Vec_Name names = utils::split("-", saltname);
      size_t j = d.name_to_id(names[0]);
      size_t i = d.name_to_id(names[1]);
      if (node.contains("A"))
        node.at("A").get_to(m_mu[j][i].a_n);
      if (node.contains("B"))
        node.at("B").get_to(m_mu[j][i].b_n);
      if (node.contains("C"))
        node.at("C").get_to(m_mu[j][i].c_n);
      if (node.contains("t_min"))
        node.at("t_min").get_to(m_mu[j][i].t_min);
      if (node.contains("t_max"))
        node.at("t_max").get_to(m_mu[j][i].t_max);
      if (node.contains("reference"))
        node.at("reference").get_to(m_mu[j][i].reference);
    }
  }
}

std::string R_Kister_Data_Store::to_json() const {
  nlohmann::json dat = nlohmann::json::parse(d.to_json());
  dat["MSTDBTP"]["estimates"]["RK"]["Density"] =
      nlohmann::json::parse(rk_to_json(m_rho));
  dat["MSTDBTP"]["estimates"]["RK"]["Viscosity"] =
      nlohmann::json::parse(rk_to_json(m_mu));
  return dat.dump();
}

std::string R_Kister_Data_Store::rk_to_json(
    std::vector<std::vector<RK_Polynomial>> m) const {
  nlohmann::json dat;
  for (std::size_t j = 0; j < d.size(); ++j) {
    for (std::size_t i = 0; i < d.size(); ++i) {
      if (!m[j][i].a_n.empty()) {
        Vec_Name outerName = d.names(j);
        if (!outerName[0].empty()) {
          Vec_Name innerName = d.names(i);
          if (!innerName[0].empty()) {
            std::string salt = outerName[0] + std::string("-") + innerName[0];
            if (!m[j][i].a_n.empty())
              dat[salt]["A"] = m[j][i].a_n;
            if (!m[j][i].b_n.empty())
              dat[salt]["B"] = m[j][i].b_n;
            if (!m[j][i].c_n.empty())
              dat[salt]["C"] = m[j][i].c_n;
            if (!(m[j][i].t_min == 0.0))
              dat[salt]["t_min"] = m[j][i].t_min;
            if (!(m[j][i].t_max == 0.0))
              dat[salt]["t_max"] = m[j][i].t_max;
            if (!m[j][i].reference.empty())
              dat[salt]["reference"] = m[j][i].reference;
          }
        }
      }
    }
  }
  return dat.dump();
}

// Return the end-members that could be used to construct a salt
Vec_Name R_Kister_Data_Store::getSaltKeys() const {
  // We don't have a list of end-members yet
  return m_keys;
}

/// TODO similarly, this is also "binary", so values are all values [0-100] -
/// [100-0] Except the esitmation error may be more extreme in select areas
std::vector<std::vector<double>>
R_Kister_Data_Store::getSaltComps(Vec_Name /* names */) const {
  // TODO
  std::vector<std::vector<double>> keys;
  return keys;
}
#ifdef SALINE_USE_HDF5
void R_Kister_Data_Store::from_h5(hid_t file) {
  // if (file < 0) {
  //     std::cout << "Failed to open file: " << dataPath << std::endl;
  // } else {
  //     std::cout << "Opened file: " << dataPath << std::endl;
  // }
  m_rho.resize(d.size(),
               std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  m_mu.resize(d.size(),
              std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  ////TODO no input for these, implemented only for downstream process
  m_cp.resize(d.size(),
              std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));
  m_k.resize(d.size(),
             std::vector<R_Kister_Data_Store::RK_Polynomial>(d.size()));

  // Start recursion from the base MSTDBTP group
  // std::cout << "Scanning HDF5 structure from " <<  dataPath <<
  // "...\n/MSTDBTP" << std::endl;
  std::string group_mstdbtp = "/MSTDBTP/estimates/RK/Density";
  hid_t mstdbtp_group = H5Gopen(file, group_mstdbtp.c_str(), H5P_DEFAULT);
  // No top level MSTDBTP group
  if (mstdbtp_group < 0) {
    std::cout << "Failed to find group: " << group_mstdbtp << std::endl;
    return;
  }

  H5G_info_t group_info;
  H5Gget_info(mstdbtp_group, &group_info);

  // Iterate through the salt objects
  // TODO it would be nice to do this //compounds.reserve(group_info.nlinks);
  for (hsize_t i = 0; i < group_info.nlinks; i++) {
    // Track down salt names
    char salt_name[256];
    H5Lget_name_by_idx(mstdbtp_group, ".", H5_INDEX_NAME, H5_ITER_NATIVE, i,
                       salt_name, sizeof(salt_name), H5P_DEFAULT);
    H5G_info_t salt_info;
    std::string salt_path = group_mstdbtp + "/";
    salt_path += salt_name;
    hid_t salt_group = H5Gopen(file, salt_path.c_str(), H5P_DEFAULT);
    Vec_Name names = utils::split("-", salt_name);
    size_t j = d.name_to_id(names[0]);
    size_t k = d.name_to_id(names[1]);
    if (H5Lexists(file, (salt_path + "/A").c_str(), H5P_DEFAULT)) {
      utils::readh5Vec(file, salt_path + "/A", m_rho[j][k].a_n);
    }
    if (H5Lexists(file, (salt_path + "/B").c_str(), H5P_DEFAULT)) {
      utils::readh5Vec(file, salt_path + "/B", m_rho[j][k].b_n);
    }
    if (H5Lexists(file, (salt_path + "/C").c_str(), H5P_DEFAULT)) {
      utils::readh5Vec(file, salt_path + "/C", m_rho[j][k].c_n);
    }
    if (H5Lexists(file, (salt_path + "/t_max").c_str(), H5P_DEFAULT)) {
      utils::readh5Scalar(file, salt_path + "/t_max", m_rho[j][k].t_min);
    }
    if (H5Lexists(file, (salt_path + "/t_min").c_str(), H5P_DEFAULT)) {
      utils::readh5Scalar(file, salt_path + "/t_min", m_rho[j][k].t_min);
    }
    if (H5Lexists(file, (salt_path + "/reference").c_str(), H5P_DEFAULT)) {
      utils::readh5Scalar(file, salt_path + "/reference",
                          m_rho[j][k].reference);
    }
    H5Gclose(salt_group);
  }
  H5Gclose(mstdbtp_group);
}
#endif
} // namespace saline
