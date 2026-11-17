%begin %{
#define Py_LIMITED_API 0x03080000
%}
%module SalinePy
%include std_string.i
%include std_vector.i
%include std_pair.i
%include exception.i

%{
#define SWIG_FILE_INIT
#include "data_store.hh"
#include "default_data_store.hh"
#include "r_kister_data_store.hh"
#include "thermophysical_properties.hh"
%}

// Instantiate templates used by example
namespace std
{
    %template(IntVector) vector<int>;
    %template(DoubleVector) vector<double>;
    %template(DoublePair) pair<double,double>;
    %template(StringVector) vector<string>;
    %template(StringVectorVector) vector<vector<string> >;
    %template(DoubleVectorVector) vector<vector<double> >;
}
// Instantiate STL templates before installing the library exception handler:
// their SWIG definitions can reset the global exception feature.
%exception {
  try {
    $action
  } catch (const std::invalid_argument &e) {
    SWIG_exception(SWIG_ValueError, e.what());
  } catch (const std::exception &e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  } catch (...) {
    SWIG_exception(SWIG_RuntimeError, "Unknown Saline error");
  }
}
%include data_store.hh
%include default_data_store.hh
%include r_kister_data_store.hh
%include thermophysical_properties.hh
