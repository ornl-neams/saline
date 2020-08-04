#include "data_store.hh"

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


// view is null if data is null and data id is valid
bool Data_Store::View::null() const
{
    return d == nullptr || (d != nullptr && !d->valid(id));
}

double Data_Store::View::cp(double temperature, double pressure) const
{
    return d->cp(id, temperature, pressure);
}

double Data_Store::View::cp_h(double enthalpy, double pressure) const
{
    return d->cp_h(id, enthalpy, pressure);
}

// viscosity
double Data_Store::View::mu(double temperature, double pressure) const
{
    return d->mu(id, temperature, pressure);
}

double Data_Store::View::mu_h(double enthalpy, double pressure) const
{
    return d->mu_h(id, enthalpy, pressure);
}

// conductivity
double Data_Store::View::k(double temperature, double pressure) const
{
    return d->k(id, temperature, pressure);
}

double Data_Store::View::k_h(double enthalpy, double pressure) const
{
    return d->k_h(id, enthalpy, pressure);
}

// density
double Data_Store::View::rho(double temperature, double pressure) const
{
    return d->rho(id, temperature, pressure);
}
double Data_Store::View::rho_h(double enthalpy, double pressure) const
{
    return d->rho_h(id, enthalpy, pressure);
}

double Data_Store::View::h_t(double temperature) const
{
    return d->h_t(id, temperature);
}

double Data_Store::View::t_h(double enthalpy) const
{
    return d->t_h(id, enthalpy);
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