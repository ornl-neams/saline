#ifndef SALINE_DEFAULT_DATA_STORE_HH
#define SALINE_DEFAULT_DATA_STORE_HH
//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   default_data_store.hh
 * \brief  Thermophyiscal Properties Data Store class implementation.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include "data_store.hh"

#include <limits>
#include <string>
#include <vector>


namespace saline
{

//===========================================================================//
/*!
 * \class Default_Data_Store
 * \brief Class implementing thermophysical property data store interface
 *
 * The default data is transcribed from 
 * 
 * Jerden, James. Molten Salt Thermophysical Properties Database Development: 2019 Update. 
 * United States: N. p., 2019. Web. doi:10.2172/1559846.
 * 
 * Units: 
 *   Conductivity  - Watts per Meter-Kelvin (W/m K)
 *   Pressure      - Kilopascal (kPa)
 *   Temperature   - Kelvin (K)
 *   Enthalpy      - Joule per Kilogram (J/kg)
 *   Viscosity     - Centipoise (cP) or milli Newton-second per square Meter (mN.s/m^2)
 *   Specific Heat - Joules per Kelvin Mole (J/K mole)
 *   Density       - Grams per Cubic Centimeter (g/cc)
 */
//===========================================================================//    
class Default_Data_Store : public Data_Store
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

    // >>> CONSTRUCTORS
    Default_Data_Store();

    // >>> ACCESSORS

    // the number of entries in the data store
    int size() const { return static_cast<int>(compounds.size());}
    
    // the constituents names in a given compound    
    Vec_Name names(Id id) const { return compounds[id].names;}

    // specific heat 
    double cp(Id, double temperature, double pressure = 101.325) const;
    double cp_h(Id, double enthalpy, double pressure = 101.325) const;

    // viscosity
    double mu(Id, double temperature, double pressure = 101.325) const;
    double mu_h(Id, double enthalpy, double pressure = 101.325) const;

    // conductivity
    double k(Id, double temperature, double pressure = 101.325) const;
    double k_h(Id, double enthalpy, double pressure = 101.325) const;

    // density
    double rho(Id, double temperature, double pressure = 101.325) const;
    double rho_h(Id, double enthalpy, double pressure = 101.325) const;

    // melting temperature
    double melt(Id id) const;

    // boiling temperature
    double boil(Id id) const;

    private:

    // convenience function for data addition
    void add(const Vec_Name& n, const Vec_Mole& mp,
              double melt, double boil,
              double rho_a, double rho_b,
              double mu_a, double mu_b,
              double k_a, double k_b,
              double cp_a, double cp_b, double cp_c, double cp_d);

    class Data
    {
      public:
        Vec_Mole mole_percents;
        double melt;
        double boil;  

        // density
        double rho_a;
        double rho_b;

        // viscosity
        double mu_a;
        double mu_b;

        // conductivity
        double k_a;
        double k_b;

        // specific heat
        double cp_a;
        double cp_b;
        double cp_c;
        double cp_d;
    };
    using Vec_Data = std::vector<Data>;
    struct Compound
    {
        Vec_Name names;
        Vec_Data data;
    };
    std::vector<Compound> compounds; 
};

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_default_data_store_hh

//---------------------------------------------------------------------------//
// end of default_data_store.hh
//---------------------------------------------------------------------------//