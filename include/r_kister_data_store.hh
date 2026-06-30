#ifndef SALINE_R_KISTER_DATA_STORE_HH
#define SALINE_R_KISTER_DATA_STORE_HH
//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   r_kister_data_store.hh
 * \brief  Thermophyiscal Properties Data Store class implementation.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//
#include <cmath>
#include <iostream>
#include <istream>
#include <limits>
#include <ostream>
#include <string>
#include <vector>

#include "data_store.hh"
#include "default_data_store.hh"

namespace saline {

class R_Kister_Data_Store : public Data_Store {
public:
  //@{
  //! Types
  using Id = std::size_t;
  using Name = std::string;
  using Vec_Id = std::vector<Id>;
  using Vec_Name = std::vector<Name>;
  using Vec_Mole = std::vector<double>;
  //@}

  // >>> CONSTRUCTORS
  R_Kister_Data_Store();
  void from_json(std::istream &inFile);
  void load(const std::string &fPath);

  [[deprecated("Use load(const std::string& fPath) to load a json instead.")]]
  void load(const std::string &rkDens, const std::string &rkVisc,
            const std::string &dfPath);
  [[deprecated("Use load(const std::string& fPath) to load a json instead.")]]
  void load(const std::string &rkDataPath, const std::string &dfPath);
  [[deprecated("Use load(const std::string& fPath) to load a json instead.")]]
  void load(std::istream &rkRhoFile, std::istream &rkMuFile,
            std::istream &inFile);
  // >>> ACCESSORS

  // the number of entries in the data store
  size_t size() const { return d.size(); }

  // the constituents names in a given compound
  Vec_Name names(Id id) const;

  // specific heat
  double cp(Id id, Id data_id, double temperature,
            double pressure = 101.325) const;
  double cp_h(Id id, Id data_id, double enthalpy,
              double pressure = 101.325) const;
  double cp_unc(Id /* id */, Id /* data_id */) const { return .2; }
  std::pair<double, double> cp_rng(Id id, Id data_id) const;
  std::string cp_ref(Id /* id */, Id /* data_id */) const { return "----"; }
  virtual bool valid_cp(Id id, Id data_id) const;

  // viscosity
  double mu(Id id, Id data_id, double temperature,
            double pressure = 101.325) const;
  double mu_h(Id id, Id data_id, double enthalpy,
              double pressure = 101.325) const;
  virtual bool valid_mu(Id id, Id data_id) const;
  double mu_unc(Id /* id */, Id /* data_id */) const { return .2; }
  std::pair<double, double> mu_rng(Id id, Id data_id) const;
  std::string mu_ref(Id /* id */, Id /* data_id */) const { return "----"; }

  // conductivity
  double k(Id id, Id data_id, double temperature,
           double pressure = 101.325) const;
  double k_h(Id id, Id data_id, double enthalpy,
             double pressure = 101.325) const;
  virtual bool valid_k(Id id, Id data_id) const;
  std::pair<double, double> k_rng(Id id, Id data_id) const;
  double k_unc(Id /* id */, Id /* data_id */) const { return .2; }
  std::string k_ref(Id /* id */, Id /* data_id */) const { return "----"; }

  // density
  double rho(Id id, Id data_id, double temperature,
             double pressure = 101.325) const;
  double rho_h(Id id, Id data_id, double enthalpy,
               double pressure = 101.325) const;
  virtual bool valid_rho(Id id, Id data_id) const;
  double rho_unc(Id /* id */, Id /* data_id */) const { return .2; }
  std::pair<double, double> rho_rng(Id id, Id data_id) const;
  std::string rho_ref(Id /* id */, Id /* data_id */) const { return "----"; }

  // surface tension
  double surfaceTension(Id id, Id data_id, double temperature,
                        double pressure = 101.325) const;
  double surfaceTension_h(Id id, Id data_id, double enthalpy,
                          double pressure = 101.325) const;
  virtual bool valid_surfaceTension(Id id, Id data_id) const;
  double surfaceTension_unc(Id /* id */, Id /* data_id */) const { return .2; }
  std::pair<double, double> surfaceTension_rng(Id id, Id data_id) const;
  std::string surfaceTension_ref(Id /* id */, Id /* data_id */) const {
    return "----";
  }

  // Speed of sound
  double speedOfSound(Id id, Id data_id, double temperature) const;
  double speedOfSound_h(Id id, Id data_id, double enthalpy,
                        double pressure = 101.325) const {
    return -1.0;
  }
  virtual bool valid_speedOfSound(Id id, Id data_id) const { return false; }
  double speedOfSound_unc(Id /* id */, Id /* data_id */) const { return .2; }
  std::pair<double, double> speedOfSound_rng(Id id, Id data_id) const {
    return {0.0, 0.0};
  }
  std::string speedOfSound_ref(Id /* id */, Id /* data_id */) const {
    return "----";
  }

  // enthalpy
  double h_t(Id id, Id data_id, double temperature) const;

  // temperature
  double t_h(Id id, Id data_id, double enthalpy) const;

  // melting temperature
  double melt(Id id, Id data_id) const;
  double melt_unc(Id /* id */, Id /* data_id */) const { return 1.0; }
  std::string melt_ref(Id /* id */, Id /* data_id */) const { return "----"; }

  // molecular weight of the salt compound
  double molecularWeight(Id id, Id data_id) const;

  // number of ions
  double n_ions(Id id, Id data_id) const;

  // anion/cation ratio
  int complexation(Id id, Id data_id) const;

  // boiling temperature
  double boil(Id id, Id data_id) const;
  double boil_unc(Id /* id */, Id /* data_id */) const { return 1.0; }
  std::string boil_ref(Id /* id */, Id /* data_id */) const { return "----"; }

  // mole percents
  const Vec_Mole &mole_percent(Id id, Id data_id) const;

  // number of constituents for the given compound
  std::size_t constituent_count(Id id) const;

  View setView(const Vec_Name &names, const Vec_Mole &mole_percents);

  // Obtain the nearest neighboring composition
  Id nearest(Id id, const Vec_Mole &mole_percent) const;

  // is the salt valid
  bool valid(Id id) const;
  bool valid(Vec_Name &names) const;
  bool valid(Name &name) const;

  // The list accessible salt names
  Vec_Name getSaltKeys() const;
  std::vector<std::vector<double>> getSaltComps(Vec_Name names) const;

  std::string to_json() const;

private:
  // Data_Store providing the base information
  Default_Data_Store d;
  Vec_Name m_keys;

  // >>> DATA
  std::vector<View> end_members;
  Vec_Name salt_name;
  Vec_Mole endMem_moleFracs;

  class RK_Polynomial {
  public:
    // Coefficients polynomial
    std::vector<double> a_n;
    std::vector<double> b_n;
    std::vector<double> c_n;

    // Bound of polynomial validity
    double t_min;
    double t_max;
    std::string algorithm;
    std::string reference;

    //  >>> ACCESSORS
    double getRK_solution(double x, double y, double temperature);
    double getRK_viscsolution(double x, double y, double temperature);
  };

  using rk_model = std::vector<std::vector<RK_Polynomial>>;
  rk_model m_rho;
  rk_model m_mu;
  rk_model m_cp;
  rk_model m_k;

  std::string rk_to_json(rk_model m) const;
  // >>> ACCESSORS
#ifdef SALINE_USE_HDF5
  void from_h5(hid_t file);
#endif

  // calculate the enthalpy interpolation tables as a function of temperature
  void setup_enthalpy_tables();
};

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_r_kister_data_store_hh

//---------------------------------------------------------------------------//
// end of r_kister_data_store.hh
//---------------------------------------------------------------------------//
