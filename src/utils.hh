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

} // end namespace util
} // end namespace saline
#endif 