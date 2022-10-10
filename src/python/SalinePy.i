%module SalinePy
%include std_string.i
%include std_vector.i

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
    %template(StringVector) vector<string>;
}
%include data_store.hh
%include default_data_store.hh
%include r_kister_data_store.hh
%include thermophysical_properties.hh
