#include "data_store.hh"

#include "saline_bug.hh"

#include <algorithm>
#include <cctype>

namespace saline
{

Data_Store::View Data_Store::view(Id id) const
{
    View v;
    if (id < size())
    {        
        v.id = id;
        v.d = this;
    }
    return v;
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given single component compound
 */
Data_Store::Id Data_Store::name_to_id(const Name& name) const
{
    return names_to_id({name});    
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given multi-component compound
 */
Data_Store::Id Data_Store::names_to_id(Vec_Name snames) const
{

    // ensure lower case comparison
    // for ( auto& name : snames)
    // {
    //     std::transform(name.begin(),name.end(), name.begin(), ::tolower);
    // }
    
    for (size_t i = 0; i < size(); ++i)
    {
        auto inames = names(i);
        if (snames == inames) return i;
    }
    return std::numeric_limits<std::size_t>::max();
}
//---------------------------------------------------------------------------//
void Data_Store::View::assign_bounds(const Vec_Mole& mp)
{
    saline_require(!mp.empty());
    saline_require(mp.size() == d->constituent_count(id));
    mole_percents = mp;
    bounds.resize(mp.size(), {0,0});

    // Loop over each compound data entry and
    // determine lower and upper records for each constituent
    for (size_t i_mp = 0; i_mp < mp.size(); ++i_mp)
    {
        bounds[i_mp] = d->extents(id, i_mp, mp[i_mp]);
    }
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given single component compound
 */
std::size_t Data_Store::View::constituent_count() const
{
      return d->constituent_count(id);
}
//---------------------------------------------------------------------------//
// view is null if data is null and data id is valid
bool Data_Store::View::null() const
{
    return d == nullptr || (d != nullptr && !d->valid(id));
}

//---------------------------------------------------------------------------//
double Data_Store::View::cp(double temperature, double pressure) const
{
    return d->cp(id, bounds[0].first, temperature, pressure);
}

//---------------------------------------------------------------------------//
double Data_Store::View::cp_h(double enthalpy, double pressure) const
{
    return d->cp_h(id, bounds[0].first, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
// viscosity
double Data_Store::View::mu(double temperature, double pressure) const
{
    return d->mu(id, bounds[0].first, temperature, pressure);
}

double Data_Store::View::mu_h(double enthalpy, double pressure) const
{
    return d->mu_h(id, bounds[0].first, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
// conductivity
double Data_Store::View::k(double temperature, double pressure) const
{
    return d->k(id, bounds[0].first, temperature, pressure);
}

double Data_Store::View::k_h(double enthalpy, double pressure) const
{
    return d->k_h(id, bounds[0].first, enthalpy, pressure);
}

// density
double Data_Store::View::rho(double temperature, double pressure) const
{
    return d->rho(id, bounds[0].first, temperature, pressure);
}
double Data_Store::View::rho_h(double enthalpy, double pressure) const
{
    return d->rho_h(id, bounds[0].first, enthalpy, pressure);
}

double Data_Store::View::h_t(double temperature) const
{
    return d->h_t(id, bounds[0].first, temperature);
}

double Data_Store::View::t_h(double enthalpy) const
{
    return d->t_h(id, bounds[0].first, enthalpy);
}

// melting temperature
double Data_Store::View::melt() const
{
    return d->melt(id);
}

// boiling temperature
double Data_Store::View::boil() const
{
    return d->boil(id);
}

} // namespace saline