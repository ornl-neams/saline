#ifndef SALINE_R_KISTER_DATA_STORE_HH
#define SALINE_R_KISTER_DATA_STORE_HH
//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   r_kister_data_store.hh
 * \brief  Thermophyiscal Properties Data Store class implementation.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include "data_store.hh"
#include "default_data_store.hh"

#include <cmath>
#include <iostream>
#include <ostream>
#include <istream>
#include <limits>
#include <string>
#include <vector>

namespace saline
{

class R_Kister_Data_Store : public Data_Store
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
    R_Kister_Data_Store();

    void load(const std::string& fPath);
    void load(std::istream& inFile);
    // >>> ACCESSORS

    // the number of entries in the data store
    size_t size() const { return d.size();}

    // the constituents names in a given compound
    Vec_Name names(Id id) const;

    // specific heat
    double cp(Id id, Id data_id, double temperature, double pressure = 101.325) const;
    double cp_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const;
    double cp_unc(Id id, Id data_id) const {return .2;}
    std::string cp_ref(Id id, Id data_id) const {return "----";}
    bool valid_cp(Id id, Id data_id) const;

    // viscosity
    double mu(Id id, Id data_id, double temperature, double pressure = 101.325) const;
    double mu_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const;
    bool valid_mu(Id id, Id data_id) const;
    double mu_unc(Id id, Id data_id) const{return .2;}
    std::string mu_ref(Id id, Id data_id) const {return "----";}

    // conductivity
    double k(Id id, Id data_id, double temperature, double pressure = 101.325) const;
    double k_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const;
    bool valid_k(Id id, Id data_id) const;
    double k_unc(Id id, Id data_id) const{return .2;}
    std::string k_ref(Id id, Id data_id) const {return "----";}

    // density
    double rho(Id id, Id data_id, double temperature, double pressure = 101.325) const;
    double rho_h(Id id, Id data_id, double enthalpy, double pressure = 101.325) const;
    bool valid_rho(Id id, Id data_id) const;
    double rho_unc(Id id, Id data_id) const{return .2;}
    std::string rho_ref(Id id, Id data_id) const {return "----";}

    // enthalpy
    double h_t(Id id, Id data_id, double temperature) const;

    // temperature
    double t_h(Id id, Id data_id, double enthalpy) const;

    // melting temperature
    double melt(Id id, Id data_id) const;
    double melt_unc(Id id, Id data_id) const{return 1.0;}
    std::string melt_ref(Id id, Id data_id) const {return "----";}

    // melting temperature
    double molecularWeight(Id id, Id data_id) const;

    // boiling temperature
    double boil(Id id, Id data_id) const;
    double boil_unc(Id id, Id data_id) const{return 1.0;}
    std::string boil_ref(Id id, Id data_id) const {return "----";}

    // mole percents
    const Vec_Mole mole_percent(Id id, Id data_id) const;

    // number of constituents for the given compound
    std::size_t constituent_count(Id id) const;

    View setView(const Vec_Name& names, const Vec_Mole& mole_percents);

    //Obtain the nearest neighboring composition
    Id nearest(Id id, const Vec_Mole& mole_percent) const;

    // is the salt valid
    bool valid(Id id) const;
    bool valid(Vec_Name& names) const;
    bool valid(Name& name) const;

    private:
        // Data_Store providing the base information
        Default_Data_Store d;

        // >>> DATA
        std::vector<View> end_members;
        Vec_Mole endMem_moleFracs;

        class RK_Polynomial
        {
            public:
            // Coefficients polynomial
            std::vector<double> a_n;
            std::vector<double> b_n;

            // Bound of polynomial validity
            double t_min;
            double t_max;
            std::string algorithm;
            std::string reference;

            //  >>> ACCESSORS
            double getRK_solution(double x, double y, double temperature);
        };

        using rk_model = std::vector<std::vector<RK_Polynomial>>;
        rk_model m_rho;
        rk_model m_mu;
        rk_model m_cp;
        rk_model m_k;

        // >>> ACCESSORS

        // calculate the enthalpy interpolation tables as a function of temperature
        void setup_enthalpy_tables();
};

} // namespace saline

//---------------------------------------------------------------------------//
#endif // saline_r_kister_data_store_hh

//---------------------------------------------------------------------------//
// end of r_kister_data_store.hh
//---------------------------------------------------------------------------//
