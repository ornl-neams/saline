#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "nlohmann/json.hpp"
#ifdef SALINE_USE_HDF5
#include "hdf5.h"
#endif

#include "default_data_store.hh"
#include "saline_bug.hh"
#include "utils.hh"

namespace saline {
//@{
//! Types
using Id = std::size_t;
using Name = std::string;
using Vec_Id = std::vector<Id>;
using Vec_Name = std::vector<Name>;
using Vec_Mole = std::vector<double>;
static constexpr double mGas_const = 8.314462618;
//@}
//----------------------------------------------------------------------------//
/*!
 * \brief translate enumeration to data note
 */
namespace {
constexpr std::pair<DataQualifier, const char *> qualifier_notes[] = {
    {DataQualifier::NONE, "None"},
    {DataQualifier::PRESSURIZED, "Pressurized"},
    {DataQualifier::SUBLIMATES, "Sublimates"},
    {DataQualifier::LIQUIDGAS, "LiquidGas"},
    {DataQualifier::NONSPECIFIC, "Uncharacterized"},
    {DataQualifier::MODELED, "Modeled"},
    {DataQualifier::EVALUATED, "Evaluated"},
    {DataQualifier::REPORTED, "Reported"},
    // Retain the external format's spelling for round-trip compatibility.
    {DataQualifier::UNCERTAINTY_PENDING_EVALUATION,
     "Uncertanity needs to be further evaluated"}};

const char *qualifierNote(DataQualifier qualifier) {
  for (const auto &entry : qualifier_notes)
    if (entry.first == qualifier)
      return entry.second;
  return "None";
}
} // namespace

DataQualifier parseNote(std::string note) {
  // Also accept the corrected spelling without requiring upstream changes.
  if (note == "Uncertainty needs to be further evaluated")
    return DataQualifier::UNCERTAINTY_PENDING_EVALUATION;
  for (const auto &entry : qualifier_notes)
    if (note == entry.second)
      return entry.first;
  return DataQualifier::NONE;
}

//---------------------------------------------------------------------------//
/*!
 * \brief constructs the default data store
 *
 * This data is transcribed from
 *
 * Jerden, James. Molten Salt Thermophysical Properties Database Development:
 * 2019 Update. United States: N. p., 2019. Web. doi:10.2172/1559846.
 *
 */
Default_Data_Store::Default_Data_Store() {}

#ifdef SALINE_USE_HDF5
void Default_Data_Store::from_h5(hid_t file) {
  std::string group_mstdbtp = "/MSTDBTP/evaluated";
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
    H5Gget_info(salt_group, &salt_info);
    std::cout << salt_name << std::endl;
    for (hsize_t j = 0; j < salt_info.nlinks; j++) {
      char mole_fracs[256];
      H5Lget_name_by_idx(salt_group, ".", H5_INDEX_NAME, H5_ITER_NATIVE, j,
                         mole_fracs, sizeof(mole_fracs), H5P_DEFAULT);
      H5G_info_t mole_fracs_info;
      std::string path = salt_path + "/" + mole_fracs;

      // This does all the data reservations // TODO suboptimal
      Default_Data_Store::Data &d = getDataReference(salt_name, mole_fracs);
      herr_t ierr;
      std::vector<double> data;
      std::vector<double> range;
      std::string note;
      if (H5Lexists(file, (path + "/melt").c_str(), H5P_DEFAULT)) {
        utils::readh5Scalar(file, path + "/melt/value", d.m_melt);
        utils::readh5Scalar(file, path + "/melt/value_notes", note);
        d.m_melt_qualifier = parseNote(note);
        note == "";
        utils::readh5Scalar(file, path + "/melt/abs_uncertainty", d.m_melt_unc);
        utils::readh5Scalar(file, path + "/melt/uncertainty_notes", note);
        d.m_melt_unc_qualifier = parseNote(note);
        ierr =
            utils::readh5Scalar(file, path + "/melt/reference", d.m_melt_ref);
      }
      if (H5Lexists(file, (path + "/boil").c_str(), H5P_DEFAULT)) {
        utils::readh5Scalar(file, path + "/boil/value", d.m_boil);
        utils::readh5Scalar(file, path + "/boil/value_notes", note);
        d.m_boil_qualifier = parseNote(note);
        note == "";
        utils::readh5Scalar(file, path + "/boil/abs_uncertainty", d.m_boil_unc);
        utils::readh5Scalar(file, path + "/boil/uncertainty_notes", note);
        d.m_boil_unc_qualifier = parseNote(note);
        ierr =
            utils::readh5Scalar(file, path + "/boil/reference", d.m_boil_ref);
      }
      if (H5Lexists(file, (path + "/density").c_str(), H5P_DEFAULT)) {
        ierr = utils::readh5Vec(file, path + "/density/values", data);
        d.m_rho_a = data[0];
        d.m_rho_b = data[1];
        utils::readh5Scalar(file, path + "/density/pct_uncertainty",
                            d.m_rho_unc);
        d.m_rho_unc /= 100.0;
        utils::readh5Scalar(file, path + "/density/uncertainty_notes", note);
        d.m_rho_unc_qualifier = parseNote(note);
        ierr = utils::readh5Vec(file, path + "/density/range", range);
        d.m_rho_rng = std::make_pair(range[0], range[1]);
        ierr =
            utils::readh5Scalar(file, path + "/density/reference", d.m_rho_ref);
      }
      if (H5Lexists(file, (path + "/heat_capacity").c_str(), H5P_DEFAULT)) {
        ierr = utils::readh5Vec(file, path + "/heat_capacity/values", data);
        d.m_cp_a = data[0];
        d.m_cp_b = data[1];
        d.m_cp_c = data[2];
        d.m_cp_d = data[3];
        utils::readh5Scalar(file, path + "/heat_capacity/pct_uncertainty",
                            d.m_cp_unc);
        d.m_cp_unc /= 100.0;
        utils::readh5Scalar(file, path + "/heat_capacity/uncertainty_notes",
                            note);
        d.m_cp_unc_qualifier = parseNote(note);
        ierr = utils::readh5Vec(file, path + "/heat_capacity/range", range);
        d.m_cp_rng = std::make_pair(range[0], range[1]);
        ierr = utils::readh5Scalar(file, path + "/heat_capacity/reference",
                                   d.m_cp_ref);
      }
      if (H5Lexists(file, (path + "/thermal_conductivity").c_str(),
                    H5P_DEFAULT)) {
        ierr =
            utils::readh5Vec(file, path + "/thermal_conductivity/values", data);
        d.m_k_a = data[0];
        d.m_k_b = data[1];
        utils::readh5Scalar(
            file, path + "/thermal_conductivity/pct_uncertainty", d.m_k_unc);
        d.m_k_unc /= 100.0;
        utils::readh5Scalar(
            file, path + "/thermal_conductivity/uncertainty_notes", note);
        d.m_k_unc_qualifier = parseNote(note);
        ierr =
            utils::readh5Vec(file, path + "/thermal_conductivity/range", range);
        d.m_k_rng = std::make_pair(range[0], range[1]);
        ierr = utils::readh5Scalar(
            file, path + "/thermal_conductivity/reference", d.m_k_ref);
      }
      if (H5Lexists(file, (path + "/viscosity").c_str(), H5P_DEFAULT)) {
        ierr = utils::readh5Vec(file, path + "/viscosity/values", data);
        d.m_mu_a = data[0];
        d.m_mu_b = data[1];
        if (data.size() == 3)
          d.m_mu_c = data[2];
        else
          d.m_mu_b /= mGas_const;
        utils::readh5Scalar(file, path + "/viscosity/pct_uncertainty",
                            d.m_mu_unc);
        d.m_mu_unc /= 100.0;
        utils::readh5Scalar(file, path + "/viscosity/uncertainty_notes", note);
        d.m_mu_unc_qualifier = parseNote(note);
        ierr = utils::readh5Vec(file, path + "/viscosity/range", range);
        d.m_mu_rng = std::make_pair(range[0], range[1]);
        ierr = utils::readh5Scalar(file, path + "/viscosity/reference",
                                   d.m_mu_ref);
      }
      if (H5Lexists(file, (path + "/surface_tension").c_str(), H5P_DEFAULT)) {
        ierr = utils::readh5Vec(file, path + "/surface_tension/values", data);
        d.m_st_a = data[0];
        d.m_st_b = data[1];
        utils::readh5Scalar(file, path + "/surface_tension/pct_uncertainty",
                            d.m_st_unc);
        d.m_st_unc /= 100.0;
        utils::readh5Scalar(file, path + "/surface_tension/uncertainty_notes",
                            note);
        d.m_st_unc_qualifier = parseNote(note);
        ierr = utils::readh5Vec(file, path + "/surface_tension/range", range);
        d.m_st_rng = std::make_pair(range[0], range[1]);
        ierr = utils::readh5Scalar(file, path + "/surface_tension/reference",
                                   d.m_st_ref);
      }
    }
    H5Gclose(salt_group);
  }
  H5Gclose(mstdbtp_group);
  setup_enthalpy_tables();

  // Close the file
  H5Fclose(file);
}
#endif
//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void Default_Data_Store::load(const std::string &fPath) {
  std::ifstream inFile(fPath.data(), std::ios::binary);
  if (!inFile.is_open())
    throw std::runtime_error("Failed to open input file: " + fPath);
  if (utils::is_hdf5_file(fPath)) {
#ifdef SALINE_USE_HDF5
    hid_t file = H5Fopen(fPath.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    from_h5(file);
#else
    throw std::runtime_error("Saline is not configured to use HDF5!");
#endif
  } else if ((fPath.size() >= 5 && fPath.substr(fPath.size() - 5) == ".json") ||
             utils::sniff_json(inFile)) {
    from_json(inFile);
  } else if (inFile.is_open()) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    load(inFile);
#pragma GCC diagnostic pop
    // Make something that dumps supported file types
    std::cout << "salineWarn: Loading a csv is deprecated, use json instead!"
              << std::endl;
  } else {
    throw std::runtime_error("Falied to open input file: " + fPath);
  }
}

//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */

void Default_Data_Store::load(std::istream &inFile) {

  // Default input file has one salt entry per line
  std::string line;
  // First two lines are header information not useful here
  std::getline(inFile, line);
  std::getline(inFile, line);
  std::getline(inFile, line);
  while (std::getline(inFile, line)) {
    utils::trim(line);
    // For now if an empty line signals the end of data
    if (line.empty())
      break;

    // Skip any comments
    if (line[0] == '/' && line[1] == '/')
      continue;

    // names, id, mole_percents, tmelt, tboil, rho_a,  rho_b, mu_a, mu_b, k_a,
    // k_b, cp_a, cp_b, cp_c, cp_d
    auto tokens = utils::split(",", line);

    // trim leading and trailing white space from all tokens
    for (size_t i = 0; i < tokens.size(); ++i) {
      utils::trim(tokens[i]);
    }

    // Get the Data instance to fill in
    Default_Data_Store::Data &d = getDataReference(tokens[0], tokens[2]);

    // Molecular Weight
    parse_data_token(tokens[1], d.m_mole_weight);

    // Melt
    parse_data_qualifier(tokens[3], d.m_melt_qualifier);
    parse_data_token(tokens[3], d.m_melt);
    // Melt uncertainty
    parse_data_qualifier(tokens[4], d.m_melt_unc_qualifier);
    parse_data_token(tokens[4], d.m_melt_unc);
    // Melt Reference
    d.m_melt_ref = tokens[5];

    // Boil
    parse_data_qualifier(tokens[6], d.m_boil_qualifier);
    parse_data_token(tokens[6], d.m_boil);
    // Boil Uncertainty
    parse_data_qualifier(tokens[7], d.m_boil_unc_qualifier);
    parse_data_token(tokens[7], d.m_boil_unc);
    // Boil Reference
    d.m_boil_ref = tokens[8];

    // Density A and B as parameters for A - BT
    parse_data_token(tokens[9], d.m_rho_a);
    parse_data_token(tokens[10], d.m_rho_b);

    // Applicability range
    parse_range_token(tokens[11], d.m_rho_rng);
    // uncertainty in percent
    parse_data_qualifier(tokens[12], d.m_rho_unc_qualifier);
    parse_data_token(tokens[12], d.m_rho_unc);
    d.m_rho_unc /= 100.0;
    // reference
    d.m_rho_ref = tokens[13];

    // Viscosity A and B as parameters A*exp(B/(RT)) ... OR
    parse_data_token(tokens[14], d.m_mu_a);
    parse_data_token(tokens[15], d.m_mu_b);
    d.m_mu_b /= mGas_const;
    d.m_mu_c = std::numeric_limits<double>::quiet_NaN();
    // Test if we received inputs for two parameter viscosity model
    if (d.m_mu_a == 0.0) {
      // Since both a and b are zero try the other model
      // A, B, C for 10^(A + B/T + C/T**2)
      parse_data_token(tokens[16], d.m_mu_a);
      parse_data_token(tokens[17], d.m_mu_b);
      parse_data_token(tokens[18], d.m_mu_c);
    }
    // Applicability range
    parse_range_token(tokens[19], d.m_mu_rng);
    // uncertainty
    parse_data_qualifier(tokens[20], d.m_mu_unc_qualifier);
    parse_data_token(tokens[20], d.m_mu_unc);
    d.m_mu_unc /= 100.0;
    // reference
    d.m_mu_ref = tokens[21];

    // Thermal conductivity A and B as parameters for A + BT
    parse_data_token(tokens[22], d.m_k_a);
    parse_data_token(tokens[23], d.m_k_b);
    // Applicability range
    parse_range_token(tokens[24], d.m_k_rng);
    // uncertainty
    parse_data_qualifier(tokens[25], d.m_k_unc_qualifier);
    parse_data_token(tokens[25], d.m_k_unc);
    d.m_k_unc /= 100.0;
    // reference
    d.m_k_ref = tokens[26];

    // heat capacity A, B, C, D for A + B*T(K) + C*T-2(K) + D*T2(K)
    parse_data_token(tokens[27], d.m_cp_a);
    parse_data_token(tokens[28], d.m_cp_b);
    parse_data_token(tokens[29], d.m_cp_c);
    parse_data_token(tokens[30], d.m_cp_d);
    // Applicability range --  Not included in data

    // uncertainty
    parse_data_qualifier(tokens[31], d.m_cp_unc_qualifier);
    parse_data_token(tokens[31], d.m_cp_unc);
    d.m_cp_unc /= 100.0;
    // reference
    d.m_cp_ref = tokens[32];

    // surface tension A and B as parameters for A - BT
    parse_data_token(tokens[33], d.m_st_a);
    parse_data_token(tokens[34], d.m_st_b);
    // Applicability range
    parse_range_token(tokens[35], d.m_st_rng);
    // uncertainty
    parse_data_qualifier(tokens[36], d.m_st_unc_qualifier);
    parse_data_token(tokens[36], d.m_st_unc);
    d.m_st_unc /= 100.0;
    // reference
    d.m_st_ref = tokens[37];
  }

  setup_enthalpy_tables();
}

//----------------------------------------------------------------------------//
/*!
 * \brief setup the enthalpy 2 temperature interpolation tables
 */
void Default_Data_Store::setup_enthalpy_tables() {
  for (auto &c : compounds) {
    for (auto &d : c.data) {
      d.calc_h_t(100);
    }
  }
}

//----------------------------------------------------------------------------//
/*!
 * \brief calculate the values of the enthalpy interpolation tables
 */
void Default_Data_Store::Data::calc_h_t(size_t table_size) {
  m_h.clear();
  m_e = std::numeric_limits<double>::quiet_NaN();
  if (!valid_cp() || !std::isfinite(m_melt) || m_melt <= 0)
    return;
  double melt2 = m_melt * m_melt;
  double melt3 = m_melt * melt2;
  // E = a * melt + b/2 * melt^2  + d/3 * melt^3 - c / melt;
  m_e = m_cp_a * m_melt + m_cp_b / 2.0 * melt2 + m_cp_d / 3.0 * melt3 -
        m_cp_c / m_melt;
  if (!std::isfinite(m_boil) || m_boil <= m_melt || table_size < 2)
    return;
  m_h.resize(table_size);
  // Retain the legacy grid spacing for compatible interpolation results.
  m_dt = (m_boil - m_melt) / m_h.size();
  // h(melt) = 0
  m_h[0] = 0.0;
  for (size_t i = 1; i < m_h.size(); ++i) {
    m_h[i] = h_t(m_melt + i * m_dt);
    if (!std::isfinite(m_h[i]) || m_h[i] <= m_h[i - 1]) {
      m_h.clear();
      return;
    }
  }
}

//----------------------------------------------------------------------------//
/*!
 * \brief helper function to calculate the enthalpy integration
 */
double Default_Data_Store::Data::h_t(double t) const {
  if (!std::isfinite(m_e))
    throw std::runtime_error("Enthalpy requires heat capacity and a positive melting temperature");
  if (!std::isfinite(t) || t <= 0)
    throw std::invalid_argument("Enthalpy requires a finite positive temperature");
  double t2 = t * t;
  double t3 = t * t2;
  // h(t) = a * t + b/2 t^2 - c / t + d/3 * t^3 - E;
  return m_cp_a * t + m_cp_b / 2.0 * t2 - m_cp_c / t + m_cp_d / 3.0 * t3 - m_e;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the enthalpy for the selected compound based on temperature
 */
double Default_Data_Store::Data::h_to_t(double h) const {
  if (m_h.size() < 2)
    throw std::runtime_error("Inverse enthalpy requires an increasing enthalpy table and valid melting/boiling bounds");
  if (!std::isfinite(h))
    throw std::invalid_argument("Enthalpy must be finite");

  auto x = std::lower_bound(m_h.begin(), m_h.end(), h);

  if (x == m_h.begin())
    x = m_h.begin();
  else if (x == m_h.end())
    x = m_h.end() - 2;
  else
    --x;

  double y0 = m_melt + std::distance(m_h.begin(), x) * m_dt;
  double y1 = y0 + m_dt;
  double x0 = *x;
  x++;
  double x1 = *x;

  double temperature = y0 + (h - x0) * (y1 - y0) / (x1 - x0);
  if (!std::isfinite(temperature) || temperature <= 0)
    throw std::runtime_error("Inverse enthalpy produced an invalid temperature");
  return temperature;
}

//----------------------------------------------------------------------------//
/*!
 * \brief returns the number of compound stored in this data store
 */
std::size_t Default_Data_Store::constituent_count(Id id) const {
  saline_require(id < compounds.size());
  return compounds[id].names.size();
}
//---------------------------------------------------------------------------//
/*!
 * \brief sets up a view of the requested data
 */
Data_Store::View Default_Data_Store::setView(const Vec_Name &names,
                                             const Vec_Mole &mole_percents) {
  Id id = names_to_id(names);
  Data_Store::View m_impl;
  if (valid(id)) {
    // This uses a nearest neighbor search to set the composition
    m_impl = view(id);
    m_impl.assign_record(mole_percents);
  }
  return m_impl;
}

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves index of the nearest neighbor
 */
auto Default_Data_Store::nearest(Id id, const Vec_Mole &mole_percent) const
    -> Id {
  double min_dist = std::numeric_limits<double>::max();
  size_t i_min = 0;
  auto datas = compounds[id].data;
  for (size_t i = 0; i < datas.size(); i++) {
    auto data = datas[i];
    const auto &mps = data.mole_percents();
    double dist = utils::euclidean_distance(mole_percent, mps);
    if (dist < min_dist) {
      min_dist = dist;
      i_min = i;
    }
  }
  return i_min;
}

//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_surfaceTension(Id id, Id data_id) const {
  return compounds[id].data[data_id].valid_surfaceTension();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_speedOfSound(Id id, Id data_id) const {
  return compounds[id].data[data_id].valid_speedOfSound();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_rho(Id id, Id data_id) const {
  return compounds[id].data[data_id].valid_rho();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_mu(Id id, Id data_id) const {
  return compounds[id].data[data_id].valid_mu();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_k(Id id, Id data_id) const {
  return compounds[id].data[data_id].valid_k();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_cp(Id id, Id data_id) const {
  return compounds[id].data[data_id].valid_cp();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on
 * temperature
 */
double Default_Data_Store::cp(Id id, Id data_id, double t,
                              double /* p */) const {
  const auto &d = compounds[id].data[data_id];

  // cp(t) = a + b * T + c * T^-2 + d * T^2
  return d.cp(t);
}
double Default_Data_Store::cp_h(Id id, Id data_id, double enthalpy,
                                double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.cp_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity experimental range for the selected
 * compound
 */
std::pair<double, double> Default_Data_Store::cp_rng(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.m_cp_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity uncertainty for the selected compound
 */
double Default_Data_Store::cp_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.cp_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity reference for the selected compound
 */
std::string Default_Data_Store::cp_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.cp_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on temperature
 */
double Default_Data_Store::mu(Id id, Id data_id, double t,
                              double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.mu(t);
}
double Default_Data_Store::mu_h(Id id, Id data_id, double enthalpy,
                                double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.mu_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity experimental range for the selected compound
 */
std::pair<double, double> Default_Data_Store::mu_rng(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.m_mu_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity uncertainty for the selected compound
 */
double Default_Data_Store::mu_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.mu_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity reference for the selected compound
 */
std::string Default_Data_Store::mu_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.mu_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on
 * temperature
 */
double Default_Data_Store::k(Id id, Id data_id, double t,
                             double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.k(t);
}
double Default_Data_Store::k_h(Id id, Id data_id, double enthalpy,
                               double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.k_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected compound
 */
std::pair<double, double> Default_Data_Store::k_rng(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.m_k_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity uncertainty for the selected compound
 */
double Default_Data_Store::k_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.k_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity reference for the selected compound
 */
std::string Default_Data_Store::k_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.k_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on temperature
 */
double Default_Data_Store::rho(Id id, Id data_id, double t,
                               double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.rho(t);
}
double Default_Data_Store::rho_h(Id id, Id data_id, double enthalpy,
                                 double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.rho_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected compound
 */
std::pair<double, double> Default_Data_Store::rho_rng(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.m_rho_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density uncertainty for the selected compound
 */
double Default_Data_Store::rho_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.rho_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density reference for the selected compound
 */
std::string Default_Data_Store::rho_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.rho_ref();
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension for the selected compound based on
 * temperature
 */
double Default_Data_Store::surfaceTension(Id id, Id data_id, double t,
                                          double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.surfaceTension(t);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension for the selected compound based on
 * temperature
 */
double Default_Data_Store::speedOfSound(Id id, Id data_id, double t) const {
  const auto &d = compounds[id].data[data_id];
  return d.speedOfSound(t);
}
double Default_Data_Store::speedOfSound_h(Id id, Id data_id, double enthalpy,
                                          double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.speedOfSound_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected compound
 */
std::pair<double, double>
Default_Data_Store::speedOfSound_rng(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.m_co_rng.value_or(std::make_pair(0.0, 0.0));
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density uncertainty for the selected compound
 */
double Default_Data_Store::speedOfSound_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.speedOfSound_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density reference for the selected compound
 */
std::string Default_Data_Store::speedOfSound_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.speedOfSound_ref();
}
double Default_Data_Store::surfaceTension_h(Id id, Id data_id, double enthalpy,
                                            double /* p */) const {
  const auto &d = compounds[id].data[data_id];
  return d.surfaceTension_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension experimental range for the selected
 * compound
 */
std::pair<double, double>
Default_Data_Store::surfaceTension_rng(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.m_st_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension uncertainty for the selected compound
 */
double Default_Data_Store::surfaceTension_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.surfaceTension_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the surface tension reference for the selected compound
 */
std::string Default_Data_Store::surfaceTension_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.surfaceTension_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the enthalpy for the selected compound based on temperature
 */
double Default_Data_Store::h_t(Id id, Id data_id, double temperature) const {
  const auto &d = compounds[id].data[data_id];
  return d.h_t(temperature);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the temperature for the selected compound based on enthalpy
 */
double Default_Data_Store::t_h(Id id, Id data_id, double enthalpy) const {
  const auto &d = compounds[id].data[data_id];
  return d.h_to_t(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the uncertainty in melting temperature of the selected
 * compound
 */
double Default_Data_Store::melt_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.melt_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the melting temperature reference of the selected compound
 */
std::string Default_Data_Store::melt_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.melt_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the melting temperature of the selected compound
 */
double Default_Data_Store::melt(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.melt();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the boiling temperature of the selected compound
 */
double Default_Data_Store::boil(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.boil();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the uncertainty in boiling temperature of the selected
 * compound
 */
double Default_Data_Store::boil_unc(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.boil_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the boiling temperature reference of the selected compound
 */
std::string Default_Data_Store::boil_ref(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.boil_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
double Default_Data_Store::molecularWeight(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.molecularWeight();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
int Default_Data_Store::complexation(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.complexation();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
double Default_Data_Store::n_ions(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.n_ions();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the mole percent
 */
const Default_Data_Store::Vec_Mole &
Default_Data_Store::mole_percent(Id id, Id data_id) const {
  const auto &d = compounds[id].data[data_id];
  return d.mole_percents();
}

//----------------------------------------------------------------------------//
/*!
 *
 */
void Default_Data_Store::from_json(std::istream &inFile) {
  Default_Data_Store candidate;
  candidate.read_json_records(inFile);
  compounds.swap(candidate.compounds);
}

void Default_Data_Store::read_json_records(std::istream &inFile) {
  nlohmann::json json_in;
  std::string path = "/MSTDBTP/evaluated";
  try {
    inFile >> json_in;
    const auto &evaluated = json_in.at("MSTDBTP").at("evaluated");
    if (!evaluated.is_object() || evaluated.empty())
      throw std::runtime_error("expected a nonempty object");

  for (auto &[saltname, node] : evaluated.items()) {
    path = "/MSTDBTP/evaluated/" + saltname;
    if (!node.is_object() || node.empty())
      throw std::runtime_error("expected a nonempty composition object");
    if (node.is_object()) {
      // compositions
      for (auto &[compname, props] : node.items()) {
        path = "/MSTDBTP/evaluated/" + saltname + "/" + compname;
        if (!props.is_object())
          throw std::runtime_error("expected a property object");
        const auto record_path = path;
        // Legacy CSV may omit pure-salt fractions; JSON keys must be numeric.
        if (utils::split("-", saltname).size() == 1) {
          size_t used = 0;
          const double fraction = std::stod(compname, &used);
          if (used != compname.size() || !std::isfinite(fraction) || fraction <= 0)
            throw std::runtime_error("invalid pure-salt composition key");
        }
        for (const auto &spec : std::vector<std::pair<std::string, size_t>>{
                 {"density", 2}, {"viscosity", 2},
                 {"thermal_conductivity", 2}, {"heat_capacity", 4},
                 {"surface_tension", 2}}) {
          if (!props.contains(spec.first))
            continue;
          path = record_path + "/" + spec.first + "/values";
          const auto &values = props.at(spec.first).at("values");
          if (!values.is_array() ||
              (values.size() != spec.second &&
               !(spec.first == "viscosity" && values.size() == 3)))
            throw std::runtime_error("invalid coefficient count");
          for (const auto &value : values)
            if (!value.is_number() || !std::isfinite(value.get<double>()))
              throw std::runtime_error("coefficients must be finite numbers");
          path = record_path + "/" + spec.first + "/range";
          const auto &range = props.at(spec.first).at("range");
          if (!range.is_array() || range.size() != 2)
            throw std::runtime_error("expected two range bounds");
          for (const auto &bound : range)
            if (!bound.is_number() || !std::isfinite(bound.get<double>()) || bound.get<double>() < 0)
              throw std::runtime_error("range bounds must be finite nonnegative numbers");
          // Zero bounds occur in existing datasets as unknown-bound sentinels.
          if (range[0].get<double>() > 0 && range[1].get<double>() > 0 &&
              range[0].get<double>() > range[1].get<double>())
            throw std::runtime_error("range bounds are reversed");
          path = record_path + "/" + spec.first + "/pct_uncertainty";
          const auto &unc = props.at(spec.first).at("pct_uncertainty");
          if (!unc.is_number() || !std::isfinite(unc.get<double>()) || unc.get<double>() < 0)
            throw std::runtime_error("uncertainty must be a finite nonnegative number");
        }
        path = record_path;
        for (const auto &field : props.items()) {
          if (field.value().is_number_float() && !std::isfinite(field.value().get<double>()))
            throw std::runtime_error("Numeric values must be finite");
        }
        if (props.is_object()) {
          Default_Data_Store::Data &d = getDataReference(saltname, compname);
          std::string note;
          std::vector<double> data;

          // Required parameters for the json file
          if (props.contains("molecular_weight")) {
            props["molecular_weight"].get_to(d.m_mole_weight);
          }
          saline_insist(std::isfinite(d.m_mole_weight) && d.m_mole_weight > 0.0,
                        "'molecular_weight' is a required value for ");
          if (props.contains("number_of_ions")) {
            props["number_of_ions"].get_to(d.m_n_ions);
          }
          if (props.contains("degrees_complexation")) {
            props["degrees_complexation"].get_to(d.m_complexation);
          }

          // Things that may or may not be present
          if (props.contains("melt")) {
            props["melt"].at("value").get_to(d.m_melt);
            props["melt"].at("abs_uncertainty").get_to(d.m_melt_unc);
            props["melt"].at("reference").get_to(d.m_melt_ref);
            props["melt"].at("value_notes").get_to(note);
            d.m_melt_qualifier = parseNote(note);
            note = "";
            props["melt"].at("uncertainty_notes").get_to(note);
            d.m_melt_unc_qualifier = parseNote(note);
            d.m_melt_doi = props["melt"].value("DOI", "");
          }
          if (props.contains("boil")) {
            props["boil"].at("value").get_to(d.m_boil);
            props["boil"].at("value_notes").get_to(note);
            d.m_boil_qualifier = parseNote(note);
            note = "";
            props["boil"].at("abs_uncertainty").get_to(d.m_boil_unc);
            props["boil"].at("uncertainty_notes").get_to(note);
            d.m_boil_unc_qualifier = parseNote(note);
            props["boil"].at("reference").get_to(d.m_boil_ref);
            d.m_boil_doi = props["boil"].value("DOI", "");
          }
          if (props.contains("density")) {
            props["density"].at("values").get_to(data);
            d.m_rho_a = data[0];
            d.m_rho_b = data[1];
            props["density"].at("pct_uncertainty").get_to(d.m_rho_unc);
            d.m_rho_unc /= 100.0;
            props["density"].at("uncertainty_notes").get_to(note);
            d.m_rho_unc_qualifier = parseNote(note);
            props["density"].at("range").get_to(d.m_rho_rng);
            props["density"].at("reference").get_to(d.m_rho_ref);
            d.m_rho_doi = props["density"].value("DOI", "");
          }
          if (props.contains("viscosity")) {
            props["viscosity"].at("values").get_to(data);
            if (data.size() == 3) {
              d.m_mu_a = data[0];
              d.m_mu_b = data[1];
              d.m_mu_c = data[2];
            } else {
              d.m_mu_a = data[0];
              d.m_mu_b = data[1] / mGas_const;
            }
            props["viscosity"].at("pct_uncertainty").get_to(d.m_mu_unc);
            d.m_mu_unc /= 100.0;
            props["viscosity"].at("uncertainty_notes").get_to(note);
            d.m_mu_unc_qualifier = parseNote(note);
            props["viscosity"].at("range").get_to(d.m_mu_rng);
            props["viscosity"].at("reference").get_to(d.m_mu_ref);
            d.m_mu_doi = props["viscosity"].value("DOI", "");
          }
          if (props.contains("thermal_conductivity")) {
            props["thermal_conductivity"].at("values").get_to(data);
            d.m_k_a = data[0];
            d.m_k_b = data[1];
            props["thermal_conductivity"]
                .at("pct_uncertainty")
                .get_to(d.m_k_unc);
            d.m_k_unc /= 100.0;
            props["thermal_conductivity"].at("uncertainty_notes").get_to(note);
            d.m_k_unc_qualifier = parseNote(note);
            props["thermal_conductivity"].at("range").get_to(d.m_k_rng);
            props["thermal_conductivity"].at("reference").get_to(d.m_k_ref);
            d.m_k_doi = props["thermal_conductivity"].value("DOI", "");
          }
          if (props.contains("heat_capacity")) {
            props["heat_capacity"].at("values").get_to(data);
            d.m_cp_a = data[0];
            d.m_cp_b = data[1];
            d.m_cp_c = data[2];
            d.m_cp_d = data[3];
            props["heat_capacity"].at("pct_uncertainty").get_to(d.m_cp_unc);
            d.m_cp_unc /= 100.0;
            props["heat_capacity"].at("uncertainty_notes").get_to(note);
            d.m_cp_unc_qualifier = parseNote(note);
            props["heat_capacity"].at("range").get_to(d.m_cp_rng);
            props["heat_capacity"].at("reference").get_to(d.m_cp_ref);
            d.m_cp_doi = props["heat_capacity"].value("DOI", "");
          }
          if (props.contains("surface_tension")) {
            props["surface_tension"].at("values").get_to(data);
            d.m_st_a = data[0];
            d.m_st_b = data[1];
            props["surface_tension"].at("pct_uncertainty").get_to(d.m_st_unc);
            d.m_st_unc /= 100.0;
            props["surface_tension"].at("uncertainty_notes").get_to(note);
            d.m_st_unc_qualifier = parseNote(note);
            props["surface_tension"].at("range").get_to(d.m_st_rng);
            props["surface_tension"].at("reference").get_to(d.m_st_ref);
            d.m_st_doi = props["surface_tension"].value("DOI", "");
          }
          if (props.contains("speed_of_sound")) {
            path = record_path + "/speed_of_sound/value";
            const auto &sound = props.at("speed_of_sound");
            const auto &value = sound.at("value");
            if (!value.is_number() || !std::isfinite(value.get<double>()) ||
                value.get<double>() <= 0)
              throw std::runtime_error("speed of sound must be a finite positive number");
            value.get_to(d.m_co_a);
            // Preserve free-form notes, including the measurement state, verbatim.
            for (const auto &field : {"reference", "DOI", "value_notes", "uncertainty_notes"}) {
              if (!sound.contains(field))
                continue;
              path = record_path + "/speed_of_sound/" + field;
              auto text = sound.at(field).get<std::string>();
              if (std::string(field) == "reference") d.m_co_ref = text;
              else if (std::string(field) == "DOI") d.m_co_doi = text;
              else if (std::string(field) == "value_notes") d.m_co_value_notes = text;
              else {
                d.m_co_unc_notes = text;
                d.m_co_unc_qualifier = parseNote(text);
              }
            }
            if (sound.contains("pct_uncertainty")) {
              path = record_path + "/speed_of_sound/pct_uncertainty";
              const auto &unc = sound.at("pct_uncertainty");
              if (!unc.is_number() || !std::isfinite(unc.get<double>()) || unc.get<double>() < 0)
                throw std::runtime_error("uncertainty must be a finite nonnegative number");
              d.m_co_unc = unc.get<double>() / 100.0;
            }
            if (sound.contains("range")) {
              path = record_path + "/speed_of_sound/range";
              const auto &range = sound.at("range");
              if (!range.is_array() || range.size() != 2)
                throw std::runtime_error("expected two range bounds");
              for (const auto &bound : range)
                if (!bound.is_number() || !std::isfinite(bound.get<double>()) || bound.get<double>() < 0)
                  throw std::runtime_error("range bounds must be finite nonnegative numbers");
              const double low = range[0].get<double>();
              const double high = range[1].get<double>();
              if (low > 0 && high > 0 && low > high)
                throw std::runtime_error("range bounds are reversed");
              d.m_co_rng = std::make_pair(low, high);
            }
            path = record_path;
          }
        } else {
          std::cout << saltname << " is not a usable dataset at " << compname
                    << "!" << std::endl;
        }
      }
    } else {
      std::cout << saltname << " is not a usable dataset!" << std::endl;
    }
  }
  setup_enthalpy_tables();
  } catch (const std::exception &e) {
    throw std::runtime_error("Invalid property JSON at " + path + ": " + e.what());
  }
}

//----------------------------------------------------------------------------//
/*!
 * \brief export the data store to json
 */
std::string Default_Data_Store::to_json() const {
  nlohmann::json j;
  for (auto comp : compounds) {
    std::string key = comp.names[0];
    for (size_t i = 1; i < comp.names.size(); ++i) {
      key.append("-");
      key.append(comp.names[i]);
    }
    for (auto dat : comp.data) {
      std::ostringstream oss;
      oss << std::setprecision(10);
      if (!dat.m_mole_percents.empty()) {
        // Convert all but the last element to avoid a trailing ","
        std::copy(dat.m_mole_percents.begin(), dat.m_mole_percents.end() - 1,
                  std::ostream_iterator<double>(oss, "-"));

        // Now add the last element with no delimiter
        oss << dat.m_mole_percents.back();
      }
      to_json(dat);
      j["MSTDBTP"]["evaluated"][key][oss.str()] =
          nlohmann::json::parse(to_json(dat));
    }
  }
  return j.dump();
}
std::string Default_Data_Store::to_json_by_id(Id id, Id data_id) const {
  nlohmann::json j;
  auto dat = compounds[id].data[data_id];
  std::ostringstream oss;
  oss << std::setprecision(10);
  if (!dat.m_mole_percents.empty()) {
    // Convert all but the last element to avoid a trailing ","
    std::copy(dat.m_mole_percents.begin(), dat.m_mole_percents.end() - 1,
              std::ostream_iterator<double>(oss, "-"));

    // Now add the last element with no delimiter
    oss << dat.m_mole_percents.back();
  }
  to_json(dat);
  j = nlohmann::json::parse(to_json(dat));
  return j.dump();
}

//----------------------------------------------------------------------------//
/*!
 * \brief export data store Data object to json
 */
std::string Default_Data_Store::to_json(Default_Data_Store::Data &d) const {
  nlohmann::json j;
  j["molecular_weight"] = d.m_mole_weight;

  if (d.m_melt != 0.0) {
    j["melt"] = {
        {"value", d.m_melt},
        {"value_notes", qualifierNote(d.m_melt_qualifier)},
        {"abs_uncertainty", d.m_melt_unc},
        {"uncertainty_notes", qualifierNote(d.m_melt_unc_qualifier)},
        {"reference", d.m_melt_ref},
        {"DOI", d.m_melt_doi},
    };
  }
  if (d.m_boil != 0.0) {
    j["boil"] = {
        {"value", d.m_boil},
        {"value_notes", qualifierNote(d.m_boil_qualifier)},
        {"abs_uncertainty", d.m_boil_unc},
        {"uncertainty_notes", qualifierNote(d.m_boil_unc_qualifier)},
        {"reference", d.m_boil_ref},
        {"DOI", d.m_boil_doi},
    };
  }
  if (d.valid_rho()) {
    j["density"] = {
        {"values", {d.m_rho_a, d.m_rho_b}},
        {"range", d.m_rho_rng},
        {"pct_uncertainty", d.m_rho_unc * 100.00},
        {"uncertainty_notes", qualifierNote(d.m_rho_unc_qualifier)},
        {"reference", d.m_rho_ref},
        {"DOI", d.m_rho_doi},
    };
  }
  if (d.valid_mu()) {
    j["viscosity"] = {
        {"range", d.m_mu_rng},
        {"pct_uncertainty", d.m_mu_unc * 100.00},
        {"uncertainty_notes", qualifierNote(d.m_mu_unc_qualifier)},
        {"reference", d.m_mu_ref},
        {"DOI", d.m_mu_doi},
    };
    if (!std::isnan(d.m_mu_c)) {
      j["viscosity"]["values"] = {d.m_mu_a, d.m_mu_b, d.m_mu_c};
    } else {
      j["viscosity"]["values"] = {d.m_mu_a, d.m_mu_b * mGas_const};
    }
  }
  if (d.valid_k()) {
    j["thermal_conductivity"] = {
        {"values", {d.m_k_a, d.m_k_b}},
        {"range", d.m_k_rng},
        {"pct_uncertainty", d.m_k_unc * 100.00},
        {"uncertainty_notes", qualifierNote(d.m_k_unc_qualifier)},
        {"reference", d.m_k_ref},
        {"DOI", d.m_k_doi},
    };
  }
  if (d.valid_cp()) {
    j["heat_capacity"] = {
        {"values", {d.m_cp_a, d.m_cp_b, d.m_cp_c, d.m_cp_d}},
        {"range", d.m_cp_rng},
        {"pct_uncertainty", d.m_cp_unc * 100.00},
        {"uncertainty_notes", qualifierNote(d.m_cp_unc_qualifier)},
        {"reference", d.m_cp_ref},
        {"DOI", d.m_cp_doi},
    };
  }
  if (d.valid_surfaceTension()) {
    j["surface_tension"] = {
        {"values", {d.m_st_a, d.m_st_b}},
        {"range", d.m_st_rng},
        {"pct_uncertainty", d.m_st_unc * 100.00},
        {"uncertainty_notes", qualifierNote(d.m_st_unc_qualifier)},
        {"reference", d.m_st_ref},
        {"DOI", d.m_st_doi},
    };
  }
  if (d.valid_speedOfSound()) {
    j["speed_of_sound"] = {{"value", d.m_co_a}};
    auto &sound = j["speed_of_sound"];
    if (d.m_co_ref) sound["reference"] = *d.m_co_ref;
    if (d.m_co_doi) sound["DOI"] = *d.m_co_doi;
    if (d.m_co_value_notes) sound["value_notes"] = *d.m_co_value_notes;
    if (d.m_co_unc_notes) sound["uncertainty_notes"] = *d.m_co_unc_notes;
    if (d.m_co_unc) sound["pct_uncertainty"] = *d.m_co_unc * 100.0;
    if (d.m_co_rng) sound["range"] = *d.m_co_rng;
  }
  return j.dump();
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given single component compound
 */
Default_Data_Store::Id Default_Data_Store::name_to_id(Name &name) const {
  return names_to_id({name});
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given multi-component compound
 */
Default_Data_Store::Id
Default_Data_Store::names_to_id(Vec_Name in_names) const {

  for (size_t i = 0; i < size(); ++i) {
    if (in_names == names(i))
      return i;
  }
  return std::numeric_limits<std::size_t>::max();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve a refernce to store/retrieve data in in hte data store
 */
Default_Data_Store::Data &
Default_Data_Store::getDataReference(std::string names, std::string fracs) {
  // Retrieve the chemical symbols from the salt description
  auto symbols = utils::split("-", names);
  if (symbols.empty() || std::any_of(symbols.begin(), symbols.end(),
                                    [](const std::string &s) { return s.empty(); }))
    throw std::invalid_argument("Composition requires nonempty species names");
  auto unique_symbols = symbols;
  std::sort(unique_symbols.begin(), unique_symbols.end());
  if (std::adjacent_find(unique_symbols.begin(), unique_symbols.end()) != unique_symbols.end())
    throw std::invalid_argument("Duplicate species in composition");

  // mole fraction of each salt
  Vec_Mole molfrac;
  if (symbols.size() == 1) {
    molfrac.push_back(1.0);
  } else {
    // Parse out the mole fractions
    for (auto mp : utils::split("-", fracs)) {
      size_t used = 0;
      double fraction = std::stod(mp, &used);
      if (used != mp.size() || !std::isfinite(fraction) || fraction < 0)
        throw std::invalid_argument("Invalid mole fraction");
      molfrac.push_back(fraction);
    }
    if (molfrac.size() != symbols.size())
      throw std::invalid_argument("Species and mole fraction counts differ");
    // Sort symbols alphabetically arranged mole fractions as needed
    auto sp = utils::getSortPermutation(symbols);
    utils::applySortPermuation_inPlace(symbols, sp);
    utils::applySortPermuation_inPlace(molfrac, sp);
  }

  // Attempt to find the salt in the existing data store
  auto it =
      find_if(compounds.begin(), compounds.end(),
              [&symbols](const Compound &c) { return c.names == symbols; });

  // Make a new salt if one cannot be found
  if (it == compounds.end()) {
    // new salt
    compounds.resize(compounds.size() + 1);
    it = std::prev(compounds.end()); // update iterator
    it->names = symbols;
  }

  // Create a space for the new data of the salt
  it->data.resize(it->data.size() + 1);
  auto &d = it->data.back();
  d.m_mole_percents = molfrac;

  return d;
}

//----------------------------------------------------------------------------//
/*!
 * \brief parse and handle quantitative data annotations
 */
void Default_Data_Store::parse_data_token(std::string &token, double &val) {
  if (all_of(token.begin(), token.end(), [](char c) { return c == '-'; })) {
    val = 0.0;
  } else if (token == "synthetic") {
    val = 0.0;
  } else {
    // sometimes data comes packaged with a less than or greater than
    if (token[0] == '<')
      token.erase(0, 1);
    if (token[0] == '>')
      token.erase(0, 1);
    val = std::stod(token);
  }
}

//----------------------------------------------------------------------------//
/*!
 * \brief parse data range information
 */
void Default_Data_Store::parse_range_token(std::string &token,
                                           std::pair<double, double> &val) {
  double stt = 0.0;
  double stp = 0.0;
  if (!all_of(token.begin(), token.end(), [](char c) { return c == '-'; })) {
    Vec_Name rng_str = utils::split("-", token);
    stt = std::stod(rng_str[0]);
    if (rng_str.size() == 2) {
      stp = std::stod(rng_str[1]);
    }
  }
  val = std::make_pair(stt, stp);
}

//----------------------------------------------------------------------------//
/*!
 * \brief parse and handle qualitative data annotations
 */
void Default_Data_Store::parse_data_qualifier(std::string &token,
                                              DataQualifier &qualifier) {
  qualifier = NONE;
  // List of possible data qualifiers
  // TODO make legend style implementation
  // static const std::list<std::string> qualifiers_list() {return
  // {"^","s","LG","*"};}
  switch (*token.rbegin()) {
  case '^':
    qualifier = DataQualifier::PRESSURIZED;
    token.pop_back();
    break;
  case 's':
    qualifier = DataQualifier::SUBLIMATES;
    token.pop_back();
    break;
  case 'G': // this actually represents "LG"
    qualifier = DataQualifier::LIQUIDGAS;
    token.pop_back();
    token.pop_back();
    break;
  case '*':
    qualifier = DataQualifier::NONSPECIFIC;
    token.pop_back();
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve salt keys from the data store
 */
Vec_Name Default_Data_Store::getSaltKeys() const {
  Vec_Name keys;
  for (auto comp : compounds) {
    std::string key = comp.names[0];
    for (size_t i = 1; i < comp.names.size(); ++i) {
      key.append("-");
      key.append(comp.names[i]);
    }
    keys.push_back(key);
  }
  return keys;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve salt compositions of a given salt from the data store
 */
std::vector<std::vector<double>>
Default_Data_Store::getSaltComps(Vec_Name names) const {
  size_t id = names_to_id(names);
  std::vector<std::vector<double>> comps;
  if (valid(id)) {
    const auto &d = compounds[id];
    for (size_t i = 0; i < d.data.size(); ++i) {
      Vec_Mole mp = d.data[i].mole_percents();
      comps.push_back(mp);
    }
  }
  return comps;
}

} // namespace saline
