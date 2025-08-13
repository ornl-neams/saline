#ifndef SALINE_UTILS_HH
#define SALINE_UTILS_HH

#include <algorithm>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#ifdef SALINE_USE_HDF5
#include <hdf5.h>
#endif

namespace saline {
namespace utils {

// Convert Celsius to Kelvin
double c2k(double celsius);

// split the 'str' on the given delimiter
std::vector<std::string> split(const std::string &delim,
                               const std::string &str);

// Calculate distance between vector a and b
// Defined as the square root of the sum of the differences squared
// Produces an exception if the rank of a and b do not match
double euclidean_distance(const std::vector<double> &a,
                          const std::vector<double> &b);

// brief helper function to test if a provided file is hdf5
bool is_hdf5_file(const std::string &filename);
#ifdef SALINE_USE_HDF5
herr_t readh5Vec(hid_t file, std::string path, std::vector<double> &data);
herr_t read_h5_scalar(hid_t file, hid_t dtype, std::string path, void *data);
herr_t readh5Scalar(hid_t file, std::string path, double &data);
herr_t readh5Scalar(hid_t file, std::string path, std::string &data);
#endif

// Calculate the nearest neighbor index set
// Returns set of indices to nearest neighbors as a function of euclidean
// distance sorted closest to farthest
std::vector<std::pair<double, size_t>>
nearest_neighbor(const std::vector<double> &a,
                 const std::vector<std::vector<double>> &neighbors);

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

// Takes a peek at each element of a vector, finding the permutations required
// to sort the vector in ascending order
template <typename T>
std::vector<std::size_t> getSortPermutation(const std::vector<T> &vec) {
  std::vector<size_t> sort_p(vec.size());
  std::iota(sort_p.begin(), sort_p.end(), 0);
  std::sort(sort_p.begin(), sort_p.end(),
            [&](std::size_t i, std::size_t j) { return vec[i] < vec[j]; });
  return sort_p;
}

// Takes a vector and a given permutation return a copy of the original vector
// with the permutations applied
template <typename T>
std::vector<T> applySortPermuation(const std::vector<T> &vec,
                                   const std::vector<std::size_t> &sort_p) {
  std::vector<T> sorted_vec(vec.size());
  std::transform(sort_p.begin(), sort_p.end(), sorted_vec.begin(),
                 [&](std::size_t i) { return vec[i]; });
  return sorted_vec;
}

// Takes a vector and a given permutation and applies the permutation on that
// vector
template <typename T>
void applySortPermuation_inPlace(std::vector<T> &vec,
                                 const std::vector<std::size_t> &p) {
  std::vector<bool> done(vec.size());
  for (std::size_t i = 0; i < vec.size(); ++i) {
    if (done[i]) {
      continue;
    }
    done[i] = true;
    std::size_t prev_j = i;
    std::size_t j = p[i];
    while (i != j) {
      std::swap(vec[prev_j], vec[j]);
      done[j] = true;
      prev_j = j;
      j = p[j];
    }
  }
}
} // namespace utils
} // end namespace saline
#endif
