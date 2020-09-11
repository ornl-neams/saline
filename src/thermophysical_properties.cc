//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   Thermophysical_Properties.cc
 * \brief  Thermophysical_Properties class definitions.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include "thermophysical_properties.hh"
#include "utils.hh"
#include "saline_bug.hh"

namespace saline
{
//---------------------------------------------------------------------------//
// CONSTRUCTORS
//---------------------------------------------------------------------------//
/*!
 * \brief Constructor.
 */
Thermophysical_Properties::Thermophysical_Properties()
    : m_data(nullptr)
{

}

//---------------------------------------------------------------------------//
// ACCESSORS 
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
/*!
 * \brief specific heat given temperature and pressure
 */
double Thermophysical_Properties::cp(double temperature, double pressure) const
{
    return m_impl.cp(temperature, pressure);
} 

//---------------------------------------------------------------------------//
/*!
 * \brief specific heat given enthalpy and pressure
 */
double Thermophysical_Properties::cp_h(double enthalpy, double pressure) const
{
    return m_impl.cp_h(enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief viscosity given temperature and pressure
 */
double Thermophysical_Properties::mu(double temperature, double pressure) const
{
    return m_impl.mu(temperature, pressure);
}  

//---------------------------------------------------------------------------//
/*!
 * \brief viscosity given enthalpy and pressure
 */
double Thermophysical_Properties::mu_h(double enthalpy, double pressure) const
{
    return m_impl.mu_h(enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief conductivity given temperature and pressure
 */
double Thermophysical_Properties::k(double temperature, double pressure) const
{
    return m_impl.k(temperature, pressure);
}   

//---------------------------------------------------------------------------//
/*!
 * \brief conductivity given enthalpy and pressure
 */
double Thermophysical_Properties::k_h(double enthalpy, double pressure) const
{
    return m_impl.k_h(enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief density given temperature and pressure
 */
double Thermophysical_Properties::rho(double temperature, double pressure) const
{
    return m_impl.rho(temperature, pressure);
}  

//---------------------------------------------------------------------------//
/*!
 * \brief density given enthalpy and pressure
 */
double Thermophysical_Properties::rho_h(double enthalpy, double pressure) const
{
    return m_impl.rho_h(enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief enthalpy given temperature
 */
double Thermophysical_Properties::h_t(double temperature) const
{
    return m_impl.h_t(temperature);
}

//---------------------------------------------------------------------------//
/*!
 * \brief temperature given enthalpy
 */
double Thermophysical_Properties::t_h(double enthalpy) const
{
    return m_impl.t_h(enthalpy);
}

// set the mole % and select the composition
bool Thermophysical_Properties::setComposition(const Vec_Name& names, 
                        const Vec_Mole& mole_percents)
{
    saline_require(names.size() == mole_percents.size());
    Id id = m_data->names_to_id(names);
    if (!m_data->valid(id)) return false;

    m_impl = m_data->view(id);
    m_impl.assign_bounds(mole_percents);
    // Alternatively, we could call utils::closest_neighbor 
    // to obtain the index of the 'closest' data record
    return !m_impl.null();
}

bool Thermophysical_Properties::setComposition(const std::string& names, 
                                            double* mole_percents,
                                            int mole_percent_count)
{
    saline_require(mole_percent_count > 0);
    auto comps = utils::split("-",names);
    if (comps.size() != mole_percent_count) return false;
    
    std::vector<double> mp(mole_percents, mole_percents + mole_percent_count);
    return setComposition(comps, mp);
}

//---------------------------------------------------------------------------//
/*!
 * \brief initialize this properties data store 
 * \returns true, iff Data_Store is non-null and contains data
 */
bool Thermophysical_Properties::initialize(Data_Store* d)
{
    if (d != nullptr && d->size() > 0)
    {
        m_data = d;
        return true;
    } 
    return false;
}

} // namespace saline