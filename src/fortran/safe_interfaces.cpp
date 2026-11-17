// Maintained exception boundary for the signatures described by cix/*.xml.
// Do not compile the generated *Interface.cpp files alongside this file.
#include "data_storeInterface.h"
#include "default_data_storeInterface.h"
#include "r_kister_data_storeInterface.h"
#include "thermophysical_propertiesInterface.h"

#include <algorithm>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace {
thread_local char last_error[2048] = {};

template<class F> auto boundary(F f) noexcept -> decltype(f()) {
  using Result = decltype(f());
  last_error[0] = '\0';
  try {
    return f();
  } catch (const std::exception &e) {
    std::snprintf(last_error, sizeof(last_error), "%s", e.what());
  } catch (...) {
    std::snprintf(last_error, sizeof(last_error), "%s", "Unknown Saline error");
  }
  if constexpr (std::is_floating_point<Result>::value)
    return std::numeric_limits<Result>::quiet_NaN();
  else if constexpr (!std::is_void<Result>::value)
    return Result{};
}

template<class T> T &object(void *p) {
  if (!p) throw std::invalid_argument("Null Saline object handle");
  return *static_cast<T *>(p);
}
template<class T> T &input(T *p) {
  if (!p) throw std::invalid_argument("Null Saline argument");
  return *p;
}
const char *text(char *p) {
  if (!p) throw std::invalid_argument("Null Saline string");
  return p;
}
} // namespace

// Returns 1 when the preceding call on this thread failed, 0 otherwise.
// Copies a null-terminated, possibly truncated message without clearing it.
extern "C" int saline_get_last_error(char *buffer, int capacity) noexcept {
  if (buffer && capacity > 0)
    std::snprintf(buffer, static_cast<size_t>(capacity), "%s", last_error);
  return last_error[0] != '\0';
}

// Macros keep the legacy ABI signatures synchronized across property families.
#define TP_STATE(name) \
extern "C" double Thermophysical_Properties_##name(void *p, double *x, double *pressure) { \
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).name(input(x), input(pressure)); }); \
}
TP_STATE(cp)
TP_STATE(cp_h)
TP_STATE(cp_kg)
TP_STATE(cp_h_kg)
TP_STATE(mu)
TP_STATE(mu_h)
TP_STATE(k)
TP_STATE(k_h)
TP_STATE(rho)
TP_STATE(rho_h)
TP_STATE(rho_kgm3)
TP_STATE(rho_h_kgm3)
#undef TP_STATE

#define TP_VALID(name) \
extern "C" bool Thermophysical_Properties_valid_##name(void *p) { \
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).valid_##name(); }); \
}
TP_VALID(cp)
TP_VALID(mu)
TP_VALID(k)
TP_VALID(rho)
#undef TP_VALID

#define TP_CONVERSION(name) \
extern "C" double Thermophysical_Properties_##name(void *p, double *x) { \
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).name(input(x)); }); \
}
TP_CONVERSION(h_t)
TP_CONVERSION(h_t_kg)
TP_CONVERSION(t_h)
TP_CONVERSION(t_h_kg)
#undef TP_CONVERSION

extern "C" double Thermophysical_Properties_t_melt(void *p) {
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).t_melt(); });
}
extern "C" double Thermophysical_Properties_t_boil(void *p) {
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).t_boil(); });
}
extern "C" void Thermophysical_Properties_init_data_store(void *p, saline::Data_Store *data) {
  boundary([&] {
    if (!object<saline::Thermophysical_Properties>(p).initialize(data))
      throw std::runtime_error("Cannot initialize properties from an empty or null store");
  });
}
extern "C" bool Thermophysical_Properties_set_composition(void *p, char *names, double *fractions, int *count) {
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).setComposition(text(names), fractions, input(count)); });
}
extern "C" bool Thermophysical_Properties_is_SaltValid(void *p, char *names, int *count) {
  return boundary([&] { return object<saline::Thermophysical_Properties>(p).isSaltValid(text(names), input(count)); });
}

#define LIFETIME(type) \
extern "C" void *type##_initialize() { \
  return boundary([]() -> void * { return new saline::type(); }); \
} \
extern "C" void type##_destroy(void *p) { \
  boundary([&] { delete static_cast<saline::type *>(p); }); \
}
LIFETIME(Thermophysical_Properties)
LIFETIME(Default_Data_Store)
LIFETIME(R_Kister_Data_Store)
#undef LIFETIME

extern "C" void Default_Data_Store_load(void *p, char *path) {
  boundary([&] { object<saline::Default_Data_Store>(p).load(text(path)); });
}
extern "C" void R_Kister_Data_Store_load(void *p, char *rho, char *mu, char *data) {
  boundary([&] { object<saline::R_Kister_Data_Store>(p).load(text(rho), text(mu), text(data)); });
}
extern "C" bool Data_Store_valid(void *p, int *id) {
  return boundary([&] { return object<saline::Data_Store>(p).valid(static_cast<size_t>(input(id))); });
}
extern "C" int Data_Store_size(void *p) {
  return boundary([&] { return static_cast<int>(object<saline::Data_Store>(p).size()); });
}
extern "C" void Data_Store_destroy(void *p) {
  boundary([&] { delete static_cast<saline::Data_Store *>(p); });
}
