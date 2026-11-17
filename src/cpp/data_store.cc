#include <algorithm>
#include <cctype>

#include "data_store.hh"
#include "saline_bug.hh"
#include "utils.hh"

namespace saline {

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given single component compound
 */
Data_Store::View Data_Store::view(Id id) const {
  View v;
  if (id < size()) {
    v.id = id;
    v.d = this;
  }
  return v;
}

//---------------------------------------------------------------------------//
/*!
 * \brief assigns the data data ID of the nearest matching composition
 */
void Data_Store::View::assign_record(const Vec_Mole &mp) {
  if (!d || !d->valid(id))
    throw std::runtime_error("Cannot select a record on an invalid view");
  if (mp.empty() || mp.size() != d->constituent_count(id))
    throw std::invalid_argument("Composition size does not match the material");

  rec_id = d->nearest(id, mp);
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given single component compound
 */
std::size_t Data_Store::View::constituent_count() const {
  if (!d || !d->valid(id))
    throw std::runtime_error("Invalid material view");
  return d->constituent_count(id);
}

//---------------------------------------------------------------------------//
/*!
 * \brief checks if the view is null
 */
// view is null if data is null and data id is valid
bool Data_Store::View::nullView() const {
  return d == nullptr || !d->valid_record(id, rec_id);
}

const Data_Store *Data_Store::View::checked_store() const {
  if (nullView())
    throw std::runtime_error("Property evaluation requires a valid selected material");
  return d;
}

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves the heat capacity for the View object based on temperature
 */
double Data_Store::View::cp(double temperature, double pressure) const {
  return checked_store()->cp(id, rec_id, temperature, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves the heat capacity for the View object based on enthalpy
 */
double Data_Store::View::cp_h(double enthalpy, double pressure) const {
  return checked_store()->cp_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Retrieves the validity of the view data
 */
bool Data_Store::View::valid_cp() const { return checked_store()->valid_cp(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the heat capacity uncertainty for the view object
 */
double Data_Store::View::cp_unc() const { return checked_store()->cp_unc(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the heat capacity experimental range for the view object
 */
std::pair<double, double> Data_Store::View::cp_rng() const {
  return checked_store()->cp_rng(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the heat capacity reference for the view object
 */
std::string Data_Store::View::cp_ref() const { return checked_store()->cp_ref(id, rec_id); }

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves the viscosity for the View object based on temperature
 */
// viscosity
double Data_Store::View::mu(double temperature, double pressure) const {
  return checked_store()->mu(id, rec_id, temperature, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves the viscosity for the View object based on enthalpy
 */
double Data_Store::View::mu_h(double enthalpy, double pressure) const {
  return checked_store()->mu_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Retrieves the validity of the view data
 */
bool Data_Store::View::valid_mu() const { return checked_store()->valid_mu(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the viscosity experimental range for the view object
 */
std::pair<double, double> Data_Store::View::mu_rng() const {
  return checked_store()->mu_rng(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the viscosity uncertainty for the view object
 */
double Data_Store::View::mu_unc() const { return checked_store()->mu_unc(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the viscosity reference for the view object
 */
std::string Data_Store::View::mu_ref() const { return checked_store()->mu_ref(id, rec_id); }

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves the thermal conductivity for the View object based on
 * temperature
 */
// conductivity
double Data_Store::View::k(double temperature, double pressure) const {
  return checked_store()->k(id, rec_id, temperature, pressure);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the thermal conductivity for the View object based on
 * enthalpy
 */
double Data_Store::View::k_h(double enthalpy, double pressure) const {
  return checked_store()->k_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Retrieves the validity of the view data
 */
bool Data_Store::View::valid_k() const { return checked_store()->valid_k(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the thermal conductivity uncertainty for the view object
 */
double Data_Store::View::k_unc() const { return checked_store()->k_unc(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the thermal conductivity experimental range for the view
 * object
 */
std::pair<double, double> Data_Store::View::k_rng() const {
  return checked_store()->k_rng(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the thermal conductivity reference for the view object
 */
std::string Data_Store::View::k_ref() const { return checked_store()->k_ref(id, rec_id); }

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the density for the View object based on temperature
 */
// density
double Data_Store::View::rho(double temperature, double pressure) const {
  return checked_store()->rho(id, rec_id, temperature, pressure);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the density for the View object based on enthalpy
 */
double Data_Store::View::rho_h(double enthalpy, double pressure) const {
  return checked_store()->rho_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Retrieves the validity of the view data
 */
bool Data_Store::View::valid_rho() const { return checked_store()->valid_rho(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the density experimental range for the view object
 */
std::pair<double, double> Data_Store::View::rho_rng() const {
  return checked_store()->rho_rng(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the density uncertainty for the view object
 */
double Data_Store::View::rho_unc() const { return checked_store()->rho_unc(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the density reference for the view object
 */
std::string Data_Store::View::rho_ref() const { return checked_store()->rho_ref(id, rec_id); }

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the surface tension for the View object based on temperature
 */
double Data_Store::View::surfaceTension(double temperature,
                                        double pressure) const {
  return checked_store()->surfaceTension(id, rec_id, temperature, pressure);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the speed of sound for the View object based on temperature
 */
double Data_Store::View::speedOfSound(double temperature) const {
  return checked_store()->speedOfSound(id, rec_id, temperature);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the speed of sound for the View object based on enthalpy
 */
double Data_Store::View::speedOfSound_h(double enthalpy,
                                        double pressure) const {
  return checked_store()->speedOfSound_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Retrieves the validity of the view data
 */
bool Data_Store::View::valid_speedOfSound() const {
  return checked_store()->valid_speedOfSound(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the speed of sound experimental range for the view object
 */
std::pair<double, double> Data_Store::View::speedOfSound_rng() const {
  return checked_store()->speedOfSound_rng(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the speed of sound uncertainty for the view object
 */
double Data_Store::View::speedOfSound_unc() const {
  return checked_store()->speedOfSound_unc(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the speed of sound reference for the view object
 */
std::string Data_Store::View::speedOfSound_ref() const {
  return checked_store()->speedOfSound_ref(id, rec_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the surface tension for the View object based on enthalpy
 */
double Data_Store::View::surfaceTension_h(double enthalpy,
                                          double pressure) const {
  return checked_store()->surfaceTension_h(id, rec_id, enthalpy, pressure);
}

//---------------------------------------------------------------------------//
/*!
 * \brief Retrieves the validity of the view data
 */
bool Data_Store::View::valid_surfaceTension() const {
  return checked_store()->valid_surfaceTension(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the surface tension experimental range for the view object
 */
std::pair<double, double> Data_Store::View::surfaceTension_rng() const {
  return checked_store()->surfaceTension_rng(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the surface tension uncertainty for the view object
 */
double Data_Store::View::surfaceTension_unc() const {
  return checked_store()->surfaceTension_unc(id, rec_id);
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the surface tension reference for the view object
 */
std::string Data_Store::View::surfaceTension_ref() const {
  return checked_store()->surfaceTension_ref(id, rec_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the enthalpy based on the temperature for the View object
 */
double Data_Store::View::h_t(double temperature) const {
  return checked_store()->h_t(id, rec_id, temperature);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the temperature based on the enthalpy for the View object
 */
double Data_Store::View::t_h(double enthalpy) const {
  return checked_store()->t_h(id, rec_id, enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the melting temperature for the View object
 */
double Data_Store::View::melt() const { return checked_store()->melt(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the melting temperature uncertainty for the view object
 */
double Data_Store::View::melt_unc() const { return checked_store()->melt_unc(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the melting temperature reference for the view object
 */
std::string Data_Store::View::melt_ref() const {
  return checked_store()->melt_ref(id, rec_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the boiling temperature for the view object
 */
double Data_Store::View::boil() const { return checked_store()->boil(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the boiling temperature uncertainty for the view object
 */
double Data_Store::View::boil_unc() const { return checked_store()->boil_unc(id, rec_id); }
//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the boiling temperature reference for the view object
 */
std::string Data_Store::View::boil_ref() const {
  return checked_store()->boil_ref(id, rec_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight of the view object
 */
double Data_Store::View::molecularWeight() const {
  return checked_store()->molecularWeight(id, rec_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight of the view object
 */
int Data_Store::View::complexation() const {
  return checked_store()->complexation(id, rec_id);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight of the view object
 */
double Data_Store::View::n_ions() const { return checked_store()->n_ions(id, rec_id); }

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves mole percent for the View
 */
const Data_Store::Vec_Mole &Data_Store::View::mole_percent() const {
  return checked_store()->mole_percent(id, rec_id);
}

} // namespace saline
