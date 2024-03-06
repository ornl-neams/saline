%begin %{
#define Py_LIMITED_API 0x03080000
%}
%module SalinePy
%include std_string.i
%include std_vector.i
%include std_pair.i

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
%include data_store.hh
%include default_data_store.hh
%include r_kister_data_store.hh
%include thermophysical_properties.hh
