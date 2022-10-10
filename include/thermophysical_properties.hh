#ifndef SALINE_THERMOPHYISCAL_PROPERTIES_HH
#define SALINE_THERMOPHYISCAL_PROPERTIES_HH
//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   Thermophysical_Properties.hh
 * \brief  Thermophyiscal Properties class definitions.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include <string>
#include <vector>
#if defined _WIN32 || defined __CYGWIN__
#include <saline-cpp_export.h>
#endif
#include "data_store.hh"

namespace saline
{

//===========================================================================//
/*!
 * \class Thermophysical_Properties
 * \brief Class for managing and interpolating a given composition's properties
 *
 * Units:
 *   Conductivity  - Watts per Meter-Kelvin (W/m K)
 *   Pressure      - Kilopascal (kPa)
 *   Temperature   - Kelvin (K)
 *   Enthalpy      - Joule per mole (J/mole)
 *   Viscosity     - Centipoise (cP) or milli Newton-second per square Meter (mN.s/m^2)
 *   Specific Heat - Joules per Kelvin Mole (J/K mole)
 *   Density       - Grams per Cubic Centimeter (g/cc)
 */
//===========================================================================//
class Thermophysical_Properties
{
  public:

    //@{
    //! Types
    using Id       = std::size_t;
    using Name     = std::string;
    using Vec_Id   = std::vector<Id>;
    using Vec_Name = std::vector<Name>;
    using Vec_Mole = std::vector<double>;
    //@}

    // Construct the cone shape
    Thermophysical_Properties();

    // >>> ACCESSORS

    // specific heat
    double cp(double temperature, double pressure = 101.325) const;
    double cp_h(double enthalpy, double pressure = 101.325) const;
    double cp_kg(double temperature, double pressure = 101.325) const;
    double cp_h_kg(double enthalpy, double pressure = 101.325) const;
    bool valid_cp() const;

    // viscosity
    double mu(double temperature, double pressure = 101.325) const;
    double mu_h(double enthalpy, double pressure = 101.325) const;
    bool valid_mu() const;

    // conductivity
    double k(double temperature, double pressure = 101.325) const;
    double k_h(double enthalpy, double pressure = 101.325) const;
    bool valid_k() const;

    // density
    double rho(double temperature, double pressure = 101.325) const;
    double rho_h(double enthalpy, double pressure = 101.325) const;
    double rho_kgm3(double temperature, double pressure = 101.325) const;
    double rho_h_kgm3(double enthalpy, double pressure = 101.325) const;
    bool valid_rho() const;

    // enthalpy
    double h_t(double temperature) const;
    double h_t_kg(double temperature) const;

    // temperature
    double t_h(double enthalpy) const;
    double t_h_kg(double enthalpy) const;

    double t_melt() const;
    double t_boil() const;

    // list of species for which properties are being tracked
    const Vec_Name& species() const {return m_comp_names;}

    // list of species mole % for which properties are being tracked
    const Vec_Mole& composition() const {return m_impl.mole_percent();}

    // set the mole % and select the composition
    bool setComposition(const Vec_Name& names,
                        const Vec_Mole& mole_percents);

    // convenience method for Fortran interface
    // names is composition names separated by '-'
    // The name count must be equal to the mole_percent_count
    // returns true, iff the names were successfully processed, counts matched
    //               AND data was found for the compound
    bool setComposition(const std::string& names, double* mole_percents,
                        int mole_percent_count);

    // interface for checking if salt is valid
    bool isSaltValid(const Vec_Name& names);
    bool isSaltValid(const std::string& names, int name_count);

    // initialize the properties data
    bool initialize(Data_Store* d);

  private:

    // the most recent species used by the client (names in order of request)
    std::vector<std::string> m_comp_names;

    // the data store backing this instance of material properties
    Data_Store* m_data;

    // Our view/specific material implementation into the data store
    Data_Store::View m_impl;
};
} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_Thermophysical_Properties_hh
//---------------------------------------------------------------------------//
// end of thermophysical_properties.hh
//---------------------------------------------------------------------------//
