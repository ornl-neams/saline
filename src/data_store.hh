#ifndef SALINE_DATA_STORE_HH
#define SALINE_DATA_STORE_HH
//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   data_store.hh
 * \brief  Thermophyiscal Properties Data Store class Interface.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include <limits>
#include <string>
#include <vector>


namespace saline
{

//===========================================================================//
/*!
 * \class Data_Store
 * \brief Interface for insulating thermophysical property queries from format
 *
 *  The Data Store consists of compounds with one or more data sets for 
 *  each property.
 * 
 *  A compound may consist of a single or multiple components (E.g., BeF2, LiF-NaF-KF) 
 * 
 * Units: 
 *   Conductivity  - Watts per Meter-Kelvin (W/m K)
 *   Pressure      - Kilopascal (kPa)
 *   Temperature   - Kelvin (K)
 *   Enthalpy      - Joule per Kilogram (J/kg)
 *   Viscosity     - Centipoise (cP) or milli Newton-second per square Meter (mN.s/m^2)
 *   Specific Heat - Joules per Kelvin Mole (J/K mole)
 *   Density       - Grams per Cubic Centimeter (g/cc)
 *
 */
//===========================================================================//    
class Data_Store
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
    // convenience accessor/view of data store
    struct View
    {
        Id id = std::numeric_limits<std::size_t>::max();
        const Data_Store* d = nullptr;

        // does view reference data
        bool null() const;

        double cp(double temperature, double pressure = 101.325) const;
        double cp_h(double enthalpy, double pressure = 101.325) const;

        // viscosity
        double mu(double temperature, double pressure = 101.325) const;
        double mu_h(double enthalpy, double pressure = 101.325) const;

        // conductivity
        double k(double temperature, double pressure = 101.325) const;
        double k_h(double enthalpy, double pressure = 101.325) const;

        // density
        double rho(double temperature, double pressure = 101.325) const;
        double rho_h(double enthalpy, double pressure = 101.325) const;

        // melting temperature
        double melt() const;

        // boiling temperature
        double boil() const;

    }; // Data_Store::View

    // >>> ACCESSORS
    View view(Id) const;    

    // the number of entries in the data store
    virtual int size() const = 0;

    // the constituents names in a given compound    
    virtual Vec_Name names(Id) const = 0;
    
    // specific heat 
    virtual double cp(Id, double temperature, double pressure = 101.325) const = 0;
    virtual double cp_h(Id, double enthalpy, double pressure = 101.325) const = 0;

    // viscosity
    virtual double mu(Id, double temperature, double pressure = 101.325) const = 0;
    virtual double mu_h(Id, double enthalpy, double pressure = 101.325) const = 0;

    // conductivity
    virtual double k(Id, double temperature, double pressure = 101.325) const = 0;
    virtual double k_h(Id, double enthalpy, double pressure = 101.325) const = 0;

    // density
    virtual double rho(Id, double temperature, double pressure = 101.325) const = 0;
    virtual double rho_h(Id, double enthalpy, double pressure = 101.325) const = 0;

    // melting temperature
    virtual double melt(Id id) const = 0;

    // boiling temperature
    virtual double boil(Id id) const = 0;

    // obtain data store id given a compound name (e.g., BeF2, NaCl)
    Id name_to_id(const Name& name) const;

    // obtain data store id given a set of compound names (e.g., LiF-NaF-KF, LiF-BeF2-ZrF4-ThF4)
    Id names_to_id(Vec_Name names) const;

    // is the id valid 
    bool valid(Id id) const {return id < size();}   
    
}; // Data_Store

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_data_store_hh
//---------------------------------------------------------------------------//
// end of data_store.hh
//---------------------------------------------------------------------------//