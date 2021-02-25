#include "data_store.hh"
#include "saline_bug.hh"
#include "utils.hh"

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

    auto sp = utils::getSortPermutation(snames);
    auto sort_names = utils::applySortPermuation(snames,sp);
    for (size_t i = 0; i < size(); ++i)
    {
        auto inames = names(i);
        if (sort_names == inames) return i;
    }
    return std::numeric_limits<std::size_t>::max();
}
//---------------------------------------------------------------------------//
void Data_Store::View::assign_record(const Vec_Mole& mp)
{
    saline_require(!mp.empty());
    saline_require(mp.size() == d->constituent_count(id));
    mole_percents = mp;

    rec_id = d->nearest(id, mp);

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
    return d->cp(id, rec_id, temperature, pressure);
}

//---------------------------------------------------------------------------//
double Data_Store::View::cp_h(double enthalpy, double pressure) const
{
    return d->cp_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
// viscosity
double Data_Store::View::mu(double temperature, double pressure) const
{
    return d->mu(id, rec_id, temperature, pressure);
}

double Data_Store::View::mu_h(double enthalpy, double pressure) const
{
    return d->mu_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
// conductivity
double Data_Store::View::k(double temperature, double pressure) const
{
    return d->k(id, rec_id, temperature, pressure);
}

double Data_Store::View::k_h(double enthalpy, double pressure) const
{
    return d->k_h(id, rec_id, enthalpy, pressure);
}

// density
double Data_Store::View::rho(double temperature, double pressure) const
{
    return d->rho(id, rec_id, temperature, pressure);
}
double Data_Store::View::rho_h(double enthalpy, double pressure) const
{
    return d->rho_h(id, rec_id, enthalpy, pressure);
}

double Data_Store::View::h_t(double temperature) const
{
    return d->h_t(id, rec_id, temperature);
}

double Data_Store::View::t_h(double enthalpy) const
{
    return d->t_h(id, rec_id, enthalpy);
}

// melting temperature
double Data_Store::View::melt() const
{
    return d->melt(id,rec_id);
}

// boiling temperature
double Data_Store::View::boil() const
{
    return d->boil(id,rec_id);
}

} // namespace saline
