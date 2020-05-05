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

#include <cmath>
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

    // enthalpy
    double h_t(Id, double temperature) const;

    // temperature 
    double t_h(Id, double enthalpy) const;

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

        Vec_Mole& mole_percents() {return m_mole_percents;};
        const Vec_Mole& mole_percents() const {return m_mole_percents;};

        // enthalpy to temperature table
        std::vector<double>& h_t() {return m_h_t;}
        const std::vector<double>& h_t() const {return m_h_t;}

        double melt() const {return m_melt;}
        double boil() const {return m_boil;}  

        // density
        double rho(double t) const {return m_rho_a - m_rho_b * t;}
        double rho_h(double h) const {return rho(h_to_t(h));}

        // density coefficients
        double rho_a() const {return m_rho_a;}
        double rho_b() const {return m_rho_b;}

        // viscosity
        double mu(double t) const {return m_mu_a * std::exp(m_mu_b / t);}
        double mu_h(double h) const {return mu(h_to_t(h));}

        // viscosity coefficients
        double mu_a() const {return m_mu_a;}
        double mu_b() const {return m_mu_b;}

        // conductivity
        double k(double t) const  {return m_k_a + m_k_b * t;}
        double k_h(double h) const {return k(h_to_t(h));}

        // conductivity coefficients
        double k_a() const {return m_k_a;}
        double k_b() const {return m_k_b;}

        // specific heat
        double cp(double t) const {double t2 = t * t;
                                  return m_cp_a + m_cp_b * t + m_cp_c * 1/(t2) + m_cp_d * t2;}
        double cp_h(double h) const {return cp(h_to_t(h));}
        
        // specific heat coefficients
        double cp_a() const {return m_cp_a;}
        double cp_b() const {return m_cp_b;}
        double cp_c() const {return m_cp_c;}
        double cp_d() const {return m_cp_d;}
        
        // populate table used enthalpy to temperature conversion
        void calc_h_t(size_t table_size);
        
        // convert enthalpy to temperature using interpolation table
        // if table has not been generated NaN is returned
        double h_to_t(double h) const;

        double h_t(double t) const;
     
        // >>> DATA

        // mole percents 
        Vec_Mole m_mole_percents;        

        double m_melt;
        double m_boil;  

        // density
        double m_rho_a;
        double m_rho_b;

        // viscosity
        double m_mu_a;
        double m_mu_b;

        // conductivity
        double m_k_a;
        double m_k_b;

        // specific heat
        double m_cp_a;
        double m_cp_b;
        double m_cp_c;
        double m_cp_d;

        // enthalpy to temperature table
        std::vector<double> m_h_t; 

        // delta temperature
        double m_dt;

        // h(t)'s E simplification (a * melt + b * melt^2  + d * melt^3 - c / melt)
        double m_e;
        
    };

    using Vec_Data = std::vector<Data>;
    struct Compound
    {
        Vec_Name names;
        Vec_Data data;
    };
  private:

    // >>> DATA 

    // All compounds in this data store
    std::vector<Compound> compounds; 

    // >>> ACCESSORS

    // calculate the enthalpy interpolation tables as a function of temperature
    void setup_enthalpy_tables();

};

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_default_data_store_hh

//---------------------------------------------------------------------------//
// end of default_data_store.hh
//---------------------------------------------------------------------------//