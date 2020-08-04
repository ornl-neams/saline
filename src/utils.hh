#ifndef SALINE_UTILS_HH
#define SALINE_UTILS_HH

#include <vector>
#include <string>

namespace saline
{
namespace utils
{

// Convert Celsius to Kelvin 
double c2k(double celsius);

// split the 'str' on the given delimiter
std::vector<std::string> split(const std::string& delim, const std::string& str);

// Calculate distance between vector a and b
// Defined as the square root of the sum of the differences squared
// Produces an exception if the rank of a and b do not match
double euclidean_distance(const std::vector<double>& a, const std::vector<double>& b);


} // end namespace util
} // end namespace saline
#endif 