#ifndef SALINE_DEFAULT_DATA_STORE_HH
#define SALINE_DEFAULT_DATA_STORE_HH
//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   default_data_store.hh
 * \brief  Thermophyiscal Properties Data Store class implementation.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include <cmath>
#include <iostream>
#include <istream>
#include <limits>
#include <list>
#include <ostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>
#ifdef SALINE_USE_HDF5
#include <hdf5.h>
#endif

#include "data_store.hh"

namespace saline {
//===========================================================================//
/*!
 * \class Default_Data_Store
 * \brief Class implementing thermophysical property data store interface
 *
 * The default data is transcribed from
 *
 * Jerden, James. Molten Salt Thermophysical Properties Database Development:
 * 2019 Update. United States: N. p., 2019. Web. doi:10.2172/1559846.
 *
 * Units:
 *   Conductivity  - Watts per Meter-Kelvin (W/m K)
 *   Pressure      - Kilopascal (kPa)
 *   Temperature   - Kelvin (K)
 *   Enthalpy      - Joule per mole (J/mole)
 *   Viscosity     - Centipoise (cP) or milli Newton-second per square Meter
 * (mN.s/m^2) Specific Heat - Joules per Kelvin Mole (J/K mole) Density       -
 * Grams per Cubic Centimeter (g/cc)
 */
//===========================================================================//

enum DataQualifier {
  NONE, // Default assignment
  // /* Represent variations on melting and boiling data
  PRESSURIZED,
  SUBLIMATES,
  LIQUIDGAS,
  // */
  NONSPECIFIC // This is targeted at uncharacterized data (uncertainty).
};

class Default_Data_Store : public Data_Store {
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
  Default_Data_Store();

  // >>> ACCESSORS

  // the number of entries in the data store
  size_t size() const { return static_cast<int>(compounds.size()); }

  // the constituents names in a given compound
  Vec_Name names(Id id) const { return compounds[id].names; }

  // specific heat
  double cp(Id id, Id data_id, double temperature,
            double pressure = 101.325) const;
  double cp_h(Id id, Id data_id, double enthalpy,
              double pressure = 101.325) const;
  virtual bool valid_cp(Id id, Id data_id) const;
  double cp_unc(Id id, Id data_id) const;
  std::pair<double, double> cp_rng(Id id, Id data_id) const;
  std::string cp_ref(Id id, Id data_id) const;

  // viscosity
  double mu(Id id, Id data_id, double temperature,
            double pressure = 101.325) const;
  double mu_h(Id id, Id data_id, double enthalpy,
              double pressure = 101.325) const;
  virtual bool valid_mu(Id id, Id data_id) const;
  double mu_unc(Id id, Id data_id) const;
  std::pair<double, double> mu_rng(Id id, Id data_id) const;
  std::string mu_ref(Id id, Id data_id) const;

  // conductivity
  double k(Id id, Id data_id, double temperature,
           double pressure = 101.325) const;
  double k_h(Id id, Id data_id, double enthalpy,
             double pressure = 101.325) const;
  virtual bool valid_k(Id id, Id data_id) const;
  double k_unc(Id id, Id data_id) const;
  std::pair<double, double> k_rng(Id id, Id data_id) const;
  std::string k_ref(Id id, Id data_id) const;

  // density
  double rho(Id id, Id data_id, double temperature,
             double pressure = 101.325) const;
  double rho_h(Id id, Id data_id, double enthalpy,
               double pressure = 101.325) const;
  virtual bool valid_rho(Id id, Id data_id) const;
  double rho_unc(Id id, Id data_id) const;
  std::pair<double, double> rho_rng(Id id, Id data_id) const;
  std::string rho_ref(Id id, Id data_id) const;

  // enthalpy
  double h_t(Id id, Id data_id, double temperature) const;

  // temperature
  double t_h(Id id, Id data_id, double enthalpy) const;

  // molecular weight
  double molecularWeight(Id id, Id data_id) const;

  // melting temperature
  double melt(Id id, Id data_id) const;
  double melt_unc(Id id, Id data_id) const;
  std::string melt_ref(Id id, Id data_id) const;

  // boiling temperature
  double boil(Id id, Id data_id) const;
  double boil_unc(Id id, Id data_id) const;
  std::string boil_ref(Id id, Id data_id) const;

  // mole percents
  const Vec_Mole &mole_percent(Id id, Id data_id) const;

  // number of constituents for the given compound
  std::size_t constituent_count(Id id) const;

  void load(const std::string &fPath);
  [[deprecated("Use from_json(std::istream& inFile) instead.")]]
  void load(std::istream &inFile);
  void from_json(nlohmann::json);
#ifdef SALINE_USE_HDF5
  void from_h5(hid_t file);
#endif

  View setView(const Vec_Name &names, const Vec_Mole &mole_percents);

  // is the id valid
  bool valid(Id id) const { return id < size(); };
  bool valid(Vec_Name &names) const { return valid(names_to_id(names)); };
  bool valid(Name &name) const { return valid(name_to_id(name)); };

  // obtain data store id given a set of compound names (e.g., LiF-NaF-KF,
  // LiF-BeF2-ZrF4-ThF4)
  Id names_to_id(Vec_Name names) const;
  // obtain data store id given a compound name (e.g., BeF2, NaCl)
  Id name_to_id(Name &name) const;

  // Obtain the nearest neighboring composition
  Id nearest(Id id, const Vec_Mole &mole_percent) const;

  // The list accessible salt names
  Vec_Name getSaltKeys() const;
  std::vector<std::vector<double>> getSaltComps(Vec_Name names) const;
  void to_json(nlohmann::json &j) const;

private:
  class Data {
  public:
    Vec_Mole &mole_percents() { return m_mole_percents; };
    const Vec_Mole &mole_percents() const { return m_mole_percents; };

    // enthalpy to temperature table
    std::vector<double> &h_t() { return m_h; }
    const std::vector<double> &h_t() const { return m_h; }

    double melt() const { return m_melt; }
    double melt_unc() const { return m_melt_unc; }
    std::string melt_ref() const { return m_melt_ref; }

    double boil() const { return m_boil; }
    double boil_unc() const { return m_boil_unc; }
    std::string boil_ref() const { return m_boil_ref; }

    // molecular weight
    double molecularWeight() const { return m_mole_weight; }

    // density
    double rho(double t) const { return m_rho_a - m_rho_b * t; }
    double rho_h(double h) const { return rho(h_to_t(h)); }
    bool valid_rho() const { return ((rho_a() != 0.0) || (rho_b() != 0.0)); }
    double rho_unc() const { return m_rho_unc; }
    std::string rho_ref() const { return m_rho_ref; }

    // density coefficients
    double rho_a() const { return m_rho_a; }
    double rho_b() const { return m_rho_b; }

    // viscosity
    double mu(double t) const {
      return std::isnan(m_mu_c)
                 ? m_mu_a * std::exp(m_mu_b / t)
                 : std::pow(10.0, m_mu_a + (m_mu_b / t) + (m_mu_c / (t * t)));
    }
    double mu_h(double h) const { return mu(h_to_t(h)); }
    bool valid_mu() const {
      return std::isnan(m_mu_c)
                 ? (mu_a() != 0.0)
                 : ((mu_a() != 0.0) || (mu_b() != 0.0) || (m_mu_c != 0.0));
    }
    double mu_unc() const { return m_mu_unc; }
    std::string mu_ref() const { return m_mu_ref; }

    // viscosity coefficients
    double mu_a() const { return m_mu_a; }
    double mu_b() const { return m_mu_b; }
    double mu_c() const { return m_mu_c; }

    // conductivity
    double k(double t) const { return m_k_a + m_k_b * t; }
    double k_h(double h) const { return k(h_to_t(h)); }
    bool valid_k() const { return ((k_a() != 0.0) || (k_b() != 0.0)); }
    double k_unc() const { return m_k_unc; }
    std::string k_ref() const { return m_k_ref; }

    // conductivity coefficients
    double k_a() const { return m_k_a; }
    double k_b() const { return m_k_b; }

    // specific heat
    double cp(double t) const {
      double t2 = t * t;
      return m_cp_a + m_cp_b * t + m_cp_c * 1 / (t2) + m_cp_d * t2;
    }
    double cp_h(double h) const { return cp(h_to_t(h)); }
    bool valid_cp() const {
      return ((cp_a() != 0.0) || (cp_b() != 0.0) || (cp_c() != 0.0) ||
              (cp_d() != 0.0));
    }
    double cp_unc() const { return m_cp_unc; }
    std::string cp_ref() const { return m_cp_ref; }

    // specific heat coefficients
    double cp_a() const { return m_cp_a; }
    double cp_b() const { return m_cp_b; }
    double cp_c() const { return m_cp_c; }
    double cp_d() const { return m_cp_d; }

    // populate table used enthalpy to temperature conversion
    void calc_h_t(size_t table_size);

    // convert enthalpy to temperature using interpolation table
    // if table has not been generated NaN is returned
    double h_to_t(double h) const;

    double h_t(double t) const;

    // >>> DATA

    // mole percents
    Vec_Mole m_mole_percents;

    // Melting Temperature
    double m_melt = 0.0;
    DataQualifier m_melt_qualifier;
    double m_melt_unc = 0.0;
    DataQualifier m_melt_unc_qualifier;
    std::string m_melt_ref;

    // Molecular Weight
    double m_mole_weight;

    // Boiling Temperature
    double m_boil = 0.0;
    DataQualifier m_boil_qualifier;
    double m_boil_unc = 0.0;
    DataQualifier m_boil_unc_qualifier;
    std::string m_boil_ref;

    // density
    double m_rho_a = 0.0;
    double m_rho_b = 0.0;
    double m_rho_unc = 0.0;
    DataQualifier m_rho_unc_qualifier;
    std::pair<double, double> m_rho_rng;
    std::string m_rho_ref;

    // viscosity
    double m_mu_a = 0.0;
    double m_mu_b = 0.0;
    double m_mu_c = std::numeric_limits<double>::quiet_NaN();
    double m_mu_unc = 0.0;
    DataQualifier m_mu_unc_qualifier;
    std::pair<double, double> m_mu_rng;
    std::string m_mu_ref;

    // conductivity
    double m_k_a = 0.0;
    double m_k_b = 0.0;
    double m_k_unc = 0.0;
    DataQualifier m_k_unc_qualifier;
    std::pair<double, double> m_k_rng;
    std::string m_k_ref;

    // specific heat
    double m_cp_a = 0.0;
    double m_cp_b = 0.0;
    double m_cp_c = 0.0;
    double m_cp_d = 0.0;
    double m_cp_unc = 0.0;
    DataQualifier m_cp_unc_qualifier;
    std::pair<double, double> m_cp_rng;
    std::string m_cp_ref;

    // enthalpy to temperature table
    std::vector<double> m_h;

    // delta temperature
    double m_dt;

    // h(t)'s E simplification (a * melt + b * melt^2  + d * melt^3 - c / melt)
    double m_e;
  };

  void to_json(nlohmann::json &j, Data &d) const;
  using Vec_Data = std::vector<Data>;
  struct Compound {
    Vec_Name names;
    Vec_Data data;
  };

private:
  // >>> DATA
  void parse_data_qualifier(std::string &, DataQualifier &);
  void parse_data_token(std::string &, double &);
  void parse_range_token(std::string &, std::pair<double, double> &);

  // All compounds in this data store
  std::vector<Compound> compounds;

  // >>> ACCESSORS

  // calculate the enthalpy interpolation tables as a function of temperature
  void setup_enthalpy_tables();

  Default_Data_Store::Data &getDataReference(std::string, std::string);
};

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_default_data_store_hh

//---------------------------------------------------------------------------//
// end of default_data_store.hh
//---------------------------------------------------------------------------//
