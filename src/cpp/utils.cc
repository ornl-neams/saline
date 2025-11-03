#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "utils.hh"
#ifdef SALINE_USE_HDF5
#include <hdf5.h>
#endif
#include "saline_bug.hh"

namespace saline {
namespace utils {

//-------------------------------------------------------------------------------------------//
// Convert celsius to kelvin
double c2k(double celsius) { return celsius + 273.15; }

//-------------------------------------------------------------------------------------------//
// Split a single logical CSV record `s` into fields using `delim`.
// Behavior:
//  - Commas (or whatever `delim` is) only split when NOT inside quotes.
//  - Inside quotes, "" becomes a literal ".
//  - Empty fields are preserved.
//  - Works with multi-character delimiters too (when outside quotes).
//  - Does NOT trim spaces and does NOT validate malformed CSV.
std::vector<std::string> split(const std::string &delim, const std::string &s) {
  std::vector<std::string> out;
  std::string cur;

  if (delim.empty()) {
    // Degenerate case: no delimiter -> whole string is one field.
    out.push_back(s);
    return out;
  }

  const char quote = '"';
  const std::size_t n = s.size();
  const std::size_t dlen = delim.size();

  bool in_quotes = false;

  for (std::size_t i = 0; i < n; ++i) {
    char c = s[i];

    if (c == quote) {
      if (in_quotes) {
        // If next is also a quote, it's an escaped quote -> emit one and skip
        // next
        if (i + 1 < n && s[i + 1] == quote) {
          cur.push_back(quote);
          ++i; // skip the second quote
        } else {
          in_quotes = false; // closing quote
        }
      } else {
        in_quotes = true; // opening quote
      }
      continue;
    }

    // Only treat delimiter as a split when we're NOT inside quotes
    if (!in_quotes && dlen <= n - i && s.compare(i, dlen, delim) == 0) {
      out.emplace_back(std::move(cur));
      cur.clear();
      i += dlen - 1; // -1 because loop will ++i
      continue;
    }

    cur.push_back(c);
  }

  if (!cur.empty()){
    out.emplace_back(std::move(cur));
  }
  return out;
} // split

//-------------------------------------------------------------------------------------------//
// Calculate the euclidean distance between vectors a and b
double euclidean_distance(const std::vector<double> &a,
                          const std::vector<double> &b) {
  saline_insist(a.size() == b.size(), "vector size must be equal!");
  saline_insist(!a.empty(), "vector must not be empty");

  double sum = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    double diff = b[i] - a[i];
    sum += diff * diff;
  }
  return std::sqrt(sum);
} // euclidean_distance

//-------------------------------------------------------------------------------------------//
// Calculate the nearest neighbor index set
std::vector<std::pair<double, size_t>>
nearest_neighbor(const std::vector<double> &a,
                 const std::vector<std::vector<double>> &neighbors) {
  std::vector<std::pair<double, size_t>> nearest;

  // Compute distance to neighbors
  for (size_t i = 0; i < neighbors.size(); ++i) {
    const auto &neighbor = neighbors[i];
    double distance = euclidean_distance(a, neighbor);
    nearest.push_back({distance, i});
  }

  // Order on shortest distance (default pair ordering on first member)
  std::sort(nearest.begin(), nearest.end());

  return nearest;
} // nearest_neighbor

//-------------------------------------------------------------------------------------------//
// trim from start (in place)
void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
          }));
}

//-------------------------------------------------------------------------------------------//
// trim from end (in place)
void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](unsigned char ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

//-------------------------------------------------------------------------------------------//
// trim from both ends (in place)
void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

/*!
 * \brief helper function to test if a provided file is hdf5
 */
bool is_hdf5_file(const std::string &filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file)
    return false;

  std::array<unsigned char, 8> signature{};
  file.read(reinterpret_cast<char *>(signature.data()), signature.size());

  const std::array<unsigned char, 8> hdf5_signature = {0x89, 'H',  'D',  'F',
                                                       0x0D, 0x0A, 0x1A, 0x0A};

  return signature == hdf5_signature;
}

/*!
 * \brief helper function to test if a provided file is json
 */
bool sniff_json(std::ifstream &in) {
  if (!in.good())
    return false;

  // Save position
  auto pos = in.tellg();

  // Skip BOM if present
  unsigned char bom[3] = {0};
  in.read(reinterpret_cast<char *>(bom), 3);
  if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)) {
    // No BOM → rewind to pos
    in.clear();
    in.seekg(pos, std::ios::beg);
  }

  // Skip whitespace
  char c;
  while (in.get(c)) {
    if (!std::isspace(static_cast<unsigned char>(c))) {
      // Found first non-WS
      in.clear();
      in.seekg(pos, std::ios::beg); // restore original pos
      return (c == '{' || c == '[');
    }
  }

  // Only whitespace / empty file
  in.clear();
  in.seekg(pos, std::ios::beg);
  return false;
}

#ifdef SALINE_USE_HDF5
herr_t readh5Vec(hid_t file, std::string path, std::vector<double> &data) {
  herr_t herr = std::numeric_limits<herr_t>::max();
  if (H5Lexists(file, path.c_str(), H5P_DEFAULT)) {
    hid_t dataset_id = H5Dopen(file, path.c_str(), H5P_DEFAULT);
    hid_t dspace_id = H5Dget_space(dataset_id);
    hsize_t n_nodes = H5Sget_simple_extent_npoints(dspace_id);
    data.resize(n_nodes);
    herr = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                   &data[0]);
    H5Sclose(dspace_id);
    H5Dclose(dataset_id);
  }
  return herr;
}
herr_t read_h5_scalar(hid_t file, hid_t dtype, std::string path, void *data) {
  hid_t dataset_id = H5Dopen(file, path.c_str(), H5P_DEFAULT);
  hid_t dspace_id = H5Dget_space(dataset_id);
  herr_t herr = H5Dread(dataset_id, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
  H5Sclose(dspace_id);
  H5Dclose(dataset_id);
  return herr;
}
herr_t readh5Scalar(hid_t file, std::string path, double &data) {
  herr_t herr = std::numeric_limits<herr_t>::max();
  if (H5Lexists(file, path.c_str(), H5P_DEFAULT)) {
    herr = read_h5_scalar(file, H5T_NATIVE_DOUBLE, path, &data);
  }
  return herr;
}
herr_t readh5Scalar(hid_t file, std::string path, std::string &data) {
  herr_t herr = std::numeric_limits<herr_t>::max();
  if (H5Lexists(file, path.c_str(), H5P_DEFAULT)) {
    hid_t dataset_id = H5Dopen(file, path.c_str(), H5P_DEFAULT);
    hid_t dtype_id = H5Dget_type(dataset_id);

    char *buf;
    herr_t herr =
        H5Dread(dataset_id, dtype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &buf);
    data.assign(buf);

    H5Dclose(dataset_id);
  }
  return herr;
}
#endif

} // namespace utils
} // end namespace saline
