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
#include <utility>
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
 *   Enthalpy      - Joule per mole (J/mole)
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
    using Vec_Mole_Bounds = std::vector<std::pair<Id, Id>>;
    //@}

    virtual ~Data_Store(){}

    // convenience accessor/view of data store
    struct View
    {
        Id id = std::numeric_limits<std::size_t>::max();
        Id rec_id = std::numeric_limits<std::size_t>::max();
        const Data_Store* d = nullptr;

        // assign bounds and mole percents for given view
        void assign_record(const Vec_Mole& mole_percents);

        // the number of constituents for this compound
        // E.g., X-Y-Z has 3
        std::size_t constituent_count() const;

        // does view reference data
        bool nullView() const;

        double cp(double temperature, double pressure = 101.325) const;
        double cp_h(double enthalpy, double pressure = 101.325) const;
        bool valid_cp() const;
        double cp_unc() const;
        std::pair<double,double> cp_rng() const;
        std::string cp_ref() const;

        // viscosity
        double mu(double temperature, double pressure = 101.325) const;
        double mu_h(double enthalpy, double pressure = 101.325) const;
        bool valid_mu() const;
        double mu_unc() const;
        std::pair<double,double> mu_rng() const;
        std::string mu_ref() const;

        // conductivity
        double k(double temperature, double pressure = 101.325) const;
        double k_h(double enthalpy, double pressure = 101.325) const;
        bool valid_k() const;
        double k_unc() const;
        std::pair<double,double> k_rng() const;
        std::string k_ref() const;

        // density
        double rho(double temperature, double pressure = 101.325) const;
        double rho_h(double enthalpy, double pressure = 101.325) const;
        bool valid_rho() const;
        double rho_unc() const;
        std::pair<double,double> rho_rng() const;
        std::string rho_ref() const;

        // enthalpy given temperature
        double h_t(double t) const;

        // temperature given enthalpy
        double t_h(double ethalpy) const;

        // melting temperature
        double melt() const;
        double melt_unc() const;
        std::string melt_ref() const;

        // boiling temperature
        double boil() const;
        double boil_unc() const;
        std::string boil_ref() const;

        // molecular weight
        double molecularWeight() const;

        // mole percents
        const Vec_Mole& mole_percent() const;

    }; // Data_Store::View

    // >>> ACCESSORS
    View view(Id) const;

    // the number of entries in the data store
    virtual size_t size() const = 0;

    // the constituents names in a given compound
    virtual Vec_Name names(Id) const = 0;

    // The list accessible salt names
    virtual Vec_Name getSaltKeys() const = 0;
    virtual std::vector<std::vector<double>> getSaltComps(Vec_Name names) const = 0;

    // specific heat
    virtual double cp(Id id, Id data_id, double temperature, double pressure = 101.325) const = 0;
    virtual double cp_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const = 0;
    virtual bool valid_cp(Id id, Id data_id) const = 0;
    virtual double cp_unc(Id id, Id data_id) const = 0;
    virtual std::pair<double,double> cp_rng(Id id, Id data_id) const = 0;
    virtual std::string cp_ref(Id id, Id data_id) const = 0;

    // viscosity
    virtual double mu(Id id, Id data_id, double temperature, double pressure = 101.325) const = 0;
    virtual double mu_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const = 0;
    virtual bool valid_mu(Id id, Id data_id) const = 0;
    virtual double mu_unc(Id id, Id data_id) const = 0;
    virtual std::pair<double,double> mu_rng(Id id, Id data_id) const = 0;
    virtual std::string mu_ref(Id id, Id data_id) const = 0;

    // conductivity
    virtual double k(Id id, Id data_id, double temperature, double pressure = 101.325) const = 0;
    virtual double k_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const = 0;
    virtual bool valid_k(Id id, Id data_id) const = 0;
    virtual double k_unc(Id id, Id data_id) const = 0;
    virtual std::pair<double,double> k_rng(Id id, Id data_id) const = 0;
    virtual std::string k_ref(Id id, Id data_id) const = 0;

    // density
    virtual double rho(Id id, Id data_id, double temperature, double pressure = 101.325) const = 0;
    virtual double rho_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const = 0;
    virtual bool valid_rho(Id id, Id data_id) const = 0;
    virtual double rho_unc(Id id, Id data_id) const = 0;
    virtual std::pair<double,double> rho_rng(Id id, Id data_id) const = 0;
    virtual std::string rho_ref(Id id, Id data_id) const = 0;

    // enthalpy
    virtual double h_t(Id id, Id data_id, double temperature) const = 0;

    // temperature
    virtual double t_h(Id id, Id data_id, double enthalpy) const = 0;

    // melting temperature
    virtual double melt(Id id, Id data_id) const = 0;
    virtual double melt_unc(Id id, Id data_id) const = 0;
    virtual std::string melt_ref(Id id, Id data_id) const = 0;

    // molecular weight
    virtual double molecularWeight(Id id, Id data_id) const = 0;

    // molecular weight
    virtual const Vec_Mole& mole_percent(Id id, Id data_id) const = 0;

    // boiling temperature
    virtual double boil(Id id, Id data_id) const = 0;
    virtual double boil_unc(Id id, Id data_id) const = 0;
    virtual std::string boil_ref(Id id, Id data_id) const = 0;

    // number of constituents for the given compound
    virtual std::size_t constituent_count(Id id) const = 0;

    //Obtain the nearest neighboring composition
    virtual Id nearest(Id id, const Vec_Mole& mole_percent) const = 0;

    // sets up the data representation
    virtual View setView(const Vec_Name& names, const Vec_Mole& mole_percents) = 0;

    virtual bool valid(Vec_Name& names) const = 0;
    virtual bool valid(Name& name) const = 0;
    virtual bool valid(Id id) const = 0;

  private:

    // >>> DATA

}; // Data_Store

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_data_store_hh
//---------------------------------------------------------------------------//
// end of data_store.hh
//---------------------------------------------------------------------------//
