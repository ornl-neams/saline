#include "utils.hh"

namespace saline
{
namespace utils
{

//-------------------------------------------------------------------------------------------//
// Convert celsius to kelvin
double c2k(double celsius)
{
    return celsius + 273.15;
}

//-------------------------------------------------------------------------------------------//
// Convert celsius to kelvin
std::vector<std::string> split( const std::string& delim,
                                const std::string& str )
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string w( str );
    while( ( pos = w.find( delim ) ) != std::string::npos )
    {
        tokens.push_back( w.substr( 0, pos ) );
        w.erase( 0, pos + delim.length() );
    }
    tokens.push_back( w );  // remainder

    // if last element is empty, delete it (only if it has more than 1)
    if( tokens.size() > 1 && tokens[tokens.size() - 1].length() == 0 )
    {
        tokens.resize( tokens.size() - 1 );
    }

    return tokens;
}  // split

} // end namespace util
} // end namespace saline