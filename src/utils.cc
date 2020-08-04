#include "utils.hh"
#include "saline_bug.hh"

#include <algorithm>
#include <cmath>

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
// split str based on the given delimiter
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

//-------------------------------------------------------------------------------------------//
// Calculate the euclidean distance between vectors a and b
double euclidean_distance(const std::vector<double>& a, const std::vector<double>& b)
{
    saline_insist(a.size() == b.size(), "vector size must be equal!");
    saline_insist(!a.empty(), "vector must not be empty");

    double sum = 0;
    for( size_t i = 0; i < a.size(); ++i)
    {
        double diff = b[i] - a[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
} // euclidean_distance

//-------------------------------------------------------------------------------------------//
// Calculate the nearest neighbor index set
std::vector<std::pair<double, size_t>>  
                        nearest_neighbor(const std::vector<double>& a, 
                        const std::vector<std::vector<double>>& neighbors)
{
    std::vector<std::pair<double, size_t>> nearest;

    // Compute distance to neighbors
    for(size_t i = 0; i < neighbors.size(); ++i)
    {
        const auto& neighbor = neighbors[i];
        double distance = euclidean_distance(a, neighbor);
        nearest.push_back({distance, i});
    }

    // Order on shortest distance (default pair ordering on first member)
    std::sort(nearest.begin(), nearest.end());

    return nearest;
} // nearest_neighbor

} // end namespace util
} // end namespace saline