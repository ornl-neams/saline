//---------------------------------*-C++-*-----------------------------------//
/*!
 * \file   Thermophysical_Properties.cc
 * \brief  Thermophysical_Properties class definitions.
 * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
 */
//---------------------------------------------------------------------------//

#include "thermophysical_properties.hh"
#include <algorithm>
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
 * \brief specific heat given temperature and pressure, returns units J/(mol*K)
 */
double Thermophysical_Properties::cp(double temperature, double pressure) const
{
    return m_impl.cp(temperature, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief specific heat given enthalpy and pressure, returns units J/(mol*K)
 */
double Thermophysical_Properties::cp_h(double enthalpy, double pressure) const
{
    return m_impl.cp_h(enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief specific heat given temperature and pressure, returns units J/(kg*K)
 */
double Thermophysical_Properties::cp_kg(double temperature, double pressure) const
{
    return m_impl.cp(temperature, pressure) * ( 1000.0/m_impl.molecularWeight());
}

//---------------------------------------------------------------------------//
/*!
 * \brief specific heat given enthalpy and pressure, returns units J/(kg*K)
 */
double Thermophysical_Properties::cp_h_kg(double enthalpy, double pressure) const
{
    return m_impl.cp_h(enthalpy, pressure) * ( 1000.0/m_impl.molecularWeight());
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
 * \brief density in kg/m^3 for given temperature and pressure
 */
double Thermophysical_Properties::rho_kgm3(double temperature, double pressure) const
{
    return m_impl.rho(temperature, pressure) * 1000.0;
}

//---------------------------------------------------------------------------//
/*!
 * \brief density in kg/m^3 for given enthalpy and pressure
 */
double Thermophysical_Properties::rho_h_kgm3(double enthalpy, double pressure) const
{
    return m_impl.rho_h(enthalpy, pressure) * 1000.0;
}

//---------------------------------------------------------------------------//
/*!
 * \brief enthalpy in J/mol given temperature
 */
double Thermophysical_Properties::h_t(double temperature) const
{
    return m_impl.h_t(temperature);
}

//---------------------------------------------------------------------------//
/*!
 * \brief enthalpy in J/kg given temperature
 */
double Thermophysical_Properties::h_t_kg(double temperature) const
{
    return m_impl.h_t(temperature) * (1000.0/m_impl.molecularWeight());
}

//---------------------------------------------------------------------------//
/*!
 * \brief temperature given enthalpy in units J/mol
 */
double Thermophysical_Properties::t_h(double enthalpy) const
{
    return m_impl.t_h(enthalpy);
}

//---------------------------------------------------------------------------//
/*!
 * \brief temperature given enthalpy in units J/kg
 */
double Thermophysical_Properties::t_h_kg(double enthalpy) const
{
    return m_impl.t_h(enthalpy * (m_impl.molecularWeight()/1000.0));
}

//---------------------------------------------------------------------------//
/*!
 *  \brief returns the melting temperature of the set composition
 */
double Thermophysical_Properties::t_melt() const
{
  return m_impl.melt();
}

//---------------------------------------------------------------------------//
/*!
 *  \brief returns whether there is valid data for the property
 */
bool Thermophysical_Properties::valid_k() const
{
  return m_impl.valid_k();
}

//---------------------------------------------------------------------------//
/*!
 *  \brief returns whether there is valid data for the property
 */
bool Thermophysical_Properties::valid_mu() const
{
  return m_impl.valid_mu();
}

//---------------------------------------------------------------------------//
/*!
 *  \brief returns whether there is valid data for the property
 */
bool Thermophysical_Properties::valid_cp() const
{
  return m_impl.valid_cp();
}

//---------------------------------------------------------------------------//
/*!
 *  \brief returns whether there is valid data for the property
 */
bool Thermophysical_Properties::valid_rho() const
{
  return m_impl.valid_rho();
}

//---------------------------------------------------------------------------//
/*!
 *  \brief returns the boiling temperature of the set composition
 */
double Thermophysical_Properties::t_boil() const
{
  return m_impl.boil();
}

//---------------------------------------------------------------------------//
/*!
 * \set the mole % and select the composition
 */
bool Thermophysical_Properties::setComposition(const Vec_Name& names,
                        const Vec_Mole& mole_percents)
{
    saline_require(names.size() == mole_percents.size());

    // The data store keeps all the records sorted
    auto sp = utils::getSortPermutation(names);
    auto sort_names = utils::applySortPermuation(names,sp);
    auto sort_percents = utils::applySortPermuation(mole_percents,sp);

    m_impl = m_data->setView(sort_names,sort_percents);
    // Store values as appropriate
    if ( !m_impl.null() )
    {
      m_comp_names = names;
    } else {
      m_comp_names.clear();
    }
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

//---------------------------------------------------------------------------//
/*!
 * \brief checks that the requested salt is valid
 * \returns true, iff the salt named is included in the loaded data_store
 */
bool Thermophysical_Properties::isSaltValid(const Vec_Name& names)
{
    auto sp = utils::getSortPermutation(names);
    auto sort_names = utils::applySortPermuation(names,sp);
    return m_data->valid(sort_names);
}

bool Thermophysical_Properties::isSaltValid(const std::string& names, int name_count)
{
  saline_require(name_count > 0);
  auto comps = utils::split("-",names);
  if(comps.size() != name_count) return false;

  return isSaltValid(comps);
}

} // namespace saline
