#include "default_data_store.hh"
#include "default_data.hh"
#include "saline_bug.hh"
#include "utils.hh"

#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace saline
{
//---------------------------------------------------------------------------//
/*!
 * \brief constructs the default data store
 *
 * This data is transcribed from
 *
 * Jerden, James. Molten Salt Thermophysical Properties Database Development: 2019 Update.
 * United States: N. p., 2019. Web. doi:10.2172/1559846.
 *
 */
Default_Data_Store::Default_Data_Store()
{

}

//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void Default_Data_Store::load()
{
    std::istringstream in(default_data);
    load(in);
}

//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void Default_Data_Store::load(const std::string& fPath)
{
    std::ifstream inFile(fPath.data());
    load(inFile);
}

//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void Default_Data_Store::load(std::istream& inFile)
{
    static constexpr double mGas_const = 8.314462618;

    // Default input file has one salt entry per line
    std::string line;
    while (std::getline(inFile,line))
    {
        if( line.empty() ) continue;
        ///load

        // names, id, mole_percents, tmelt, tboil, rho_a,  rho_b, mu_a, mu_b, k_a, k_b, cp_a, cp_b, cp_c, cp_d
        auto tokens = utils::split(",",line);

        //Chemical symbols
        auto symbols = utils::split("-",tokens[0]);

        // ID = tokens[1], just skip, its currently unused

        //mole fraction of each salt
        std::vector<double> molfrac;
        if( symbols.size() == 1)
        {
            //This is for pure salt and might benefit from some error checking
            molfrac.push_back(1.0);
        }
        else
        {
            // Parse out the mole fractions
            for(auto mp : utils::split("-",tokens[2]))
            {
                molfrac.push_back(std::stod(mp));
            }
            //Sort symbols alphabetically arranged mole fractions as needed
            auto sp = utils::getSortPermutation(symbols);
            utils::applySortPermuation_inPlace(symbols,sp);
            utils::applySortPermuation_inPlace(molfrac,sp);
        }

        // Find reference to the salt if it exists
        auto it = find_if(compounds.begin(),compounds.end(),
                [&symbols](const Compound& c) {return c.names == symbols;});

        // Make a new salt if one cannot be found
        if (it == compounds.end())
        {
            // new salt
            compounds.resize(compounds.size()+1);
            it = std::prev(compounds.end());       //update iterator
            it->names = symbols;
        }

        // Create a space for the new data of the salt
        it->data.resize(it->data.size()+1);
        auto& d = it->data.back();
        d.m_mole_percents = molfrac;

        //Melt
        try {
            d.m_melt = std::stod(tokens[3]);
        }
        catch(const std::invalid_argument& ia) {
            d.m_melt = 0.0;
        }
        //Melt uncertainty
        //Melt Reference

        //Boil
        try {
            d.m_boil = std::stof(tokens[6]);
        }
        catch(const std::invalid_argument& ia) {
            d.m_boil = 0.0;
        }
        //Boil Uncertainty
        //Boil Reference

        //Density A and B as parameters for A - BT
        try {
            d.m_rho_a = std::stof(tokens[9]);
        }
        catch(const std::invalid_argument& ia) {
            d.m_rho_a = 0.0;
        }

        try {
            d.m_rho_b = std::stof(tokens[10]);
        }
        catch(const std::invalid_argument& ia) {
            d.m_rho_b = 0.0;
        }
        //Applicability range
        std::vector<std::string> tmps = utils::split("-",tokens[11]);

        //uncertainty
        //reference

        //Viscosity A and B as parameters A*exp(B/(RT)) ... OR
        try {
            d.m_mu_a = std::stof(tokens[14]);
            d.m_mu_b = std::stof(tokens[15])/mGas_const;
            d.m_mu_c = 0.0;
        }
        catch(const std::invalid_argument& ia)
        {
            // The two parameter version wasn't valid try the three version
            try{
                // A, B, C for 10^(A + B/T + C/T**2)
                d.m_mu_a = std::stof(tokens[16]);
                d.m_mu_b = std::stof(tokens[17]);
                d.m_mu_c = std::stof(tokens[18]);
            }
            catch(const std::invalid_argument& ia)
            {
                d.m_mu_a = 0.0;
                d.m_mu_b = 0.0;
                d.m_mu_c = 0.0;
            }
        }
        //Applicability range
        //uncertainty
        //reference

        //Thermal conductivity A and B as parameters for A + BT
        try {
            d.m_k_a  = std::stof(tokens[22]);
        }
        catch(const std::invalid_argument& ia) {
            d.m_k_a  = 0.0;
        }
        try {
            d.m_k_b  = std::stof(tokens[23]);
        }
        catch(const std::invalid_argument& ia) {
            d.m_k_a  = 0.0;
        }
        //Applicability range
        //uncertainty
        //reference

        //heat capacity A, B, C, D for A + B*T(K) + C*T-2(K) + D*T2(K)
        try {
            d.m_cp_a = std::stof(tokens[27]);
        }
        catch(const std::invalid_argument& ia)
        {
            d.m_cp_a = 0.0;
        }
        try {
            d.m_cp_b = std::stof(tokens[28]);
        }
        catch(const std::invalid_argument& ia)
        {
            d.m_cp_b = 0.0;
        }
        try {
            d.m_cp_c = std::stof(tokens[29]);
        }
        catch(const std::invalid_argument& ia)
        {
            d.m_cp_c = 0.0;
        }
        try {
            d.m_cp_d = std::stof(tokens[30]);
        }
        catch(const std::invalid_argument& ia)
        {
            d.m_cp_d = 0.0;
        }
        //Applicability range
        //uncertainty
        //reference

    }

    setup_enthalpy_tables();
}

// setup the enthalpy 2 temperature interpolation tables
void Default_Data_Store::setup_enthalpy_tables()
{
    for (auto& c : compounds)
    {
        for (auto &d  : c.data)
        {
            d.calc_h_t(100);
        }
    }
}

void Default_Data_Store::Data::calc_h_t(size_t table_size)
{
    if (m_melt == 0 || m_boil == 0) return;
    m_h.resize(table_size);
    m_dt = (m_boil - m_melt) / m_h.size();
    double melt2 = m_melt * m_melt;
    double melt3 = m_melt * melt2;
    // E = a * melt + b/2 * melt^2  + d/3 * melt^3 - c / melt;
    m_e = m_cp_a * m_melt + m_cp_b / 2.0 * melt2 + m_cp_d / 3.0 * melt3 - m_cp_c / m_melt;
    // h(melt) = 0
    m_h[0] = 0.0;
    size_t i = 1;
    for (double t = m_melt + m_dt; t < m_boil && i < m_h.size(); t+=m_dt, ++i)
    {
        m_h[i] = h_t(t);
    }
}

double Default_Data_Store::Data::h_t(double t) const
{
    double t2 = t * t;
    double t3 = t * t2;
    // h(t) = a * t + b/2 t^2 - c / t + d/3 * t^3 - E;
    return m_cp_a * t + m_cp_b / 2.0 * t2 - m_cp_c / t + m_cp_d / 3.0 * t3 - m_e;
}

// enthalpy to temperature
double Default_Data_Store::Data::h_to_t(double h) const
{
    if (m_h.empty()) return std::numeric_limits<double>::quiet_NaN();

    auto x = std::lower_bound(m_h.begin(), m_h.end(), h);

    if (h < 0) x = m_h.begin();

    else if (x == m_h.end() ) x = m_h.end()-2;

    double y0 = m_melt + std::distance(m_h.begin(), x) * m_dt;
    double y1 = y0 + m_dt;
    double x0 = *x; x++;
    double x1 = *x;

    return y0 + (h-x0) * (y1-y0) / (x1-x0);
}

std::size_t Default_Data_Store::constituent_count(Id id) const
{
    saline_require(id < compounds.size());
    saline_check(compounds[id].data.size() == compounds[id].names.size());
    return compounds[id].data.size();
}
//---------------------------------------------------------------------------//
/*!
 * \brief sets up a view of the requested data
 */
Data_Store::View Default_Data_Store::setView(const Vec_Name& names, const Vec_Mole& mole_percents)
{
    auto sp = utils::getSortPermutation(names);
    auto names_sort = utils::applySortPermuation(names,sp);
    Id id = names_to_id(names_sort);
    Data_Store::View m_impl;
    if( valid(id) )
    {
        // This uses a nearest neighbor search to set the composition
        m_impl = view(id);
        auto molfrac = utils::applySortPermuation(mole_percents,sp);
        m_impl.assign_record(molfrac);
    }
    return m_impl;
}

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves index of the nearest neighbor
 */
auto Default_Data_Store::nearest(Id id, const Vec_Mole& mole_percent) const
 -> Id
{
    double min_dist = std::numeric_limits<double>::max();
    size_t i_min = 0;
    for(size_t i =0; i < compounds[id].data.size(); i++)
    {
        auto datas = compounds[id].data;
        auto data = datas[i];
        const auto& mps = data.mole_percents();
        double dist = utils::euclidean_distance(mole_percent,mps);
        if (dist < min_dist)
        {
            min_dist = dist;
            i_min = i;
        }
    }
    return i_min;
}

//---------------------------------------------------------------------------//
/*!
 * \brief retrieves a pair indicating the index of the highest and lowest mole
 * fraction of a particular constituent.
 */
auto Default_Data_Store::extents(Id id, Id mp_id, double mp) const
 -> std::pair<Id, Id>
{
    saline_require(id < compounds.size());
    saline_require(mp_id < compounds[id].data.size());

    size_t i_min = 0;
    size_t i_max = 0;
    bool max_assigned = false;
    bool min_assigned = false;
    // loop over data records looking for min and max percents for the given constituent (mp_id)
    for (size_t i = 0, count = compounds[id].data.size(); i < count; ++i)
    {
        auto datas = compounds[id].data;
        auto data = datas[i];
        const auto& mps = data.mole_percents();
        // is the new entry (mps[mp_id]) greater or equal to search term (mp)
        bool is_max_gte = mps[mp_id] >= mp;
        if ((is_max_gte && !max_assigned) ||
            (max_assigned && is_max_gte && mps[mp_id] <= datas[i_max].mole_percents()[mp_id]))
        {
            i_max = i;
            max_assigned = true;
        }
        // is the new entry (mps[mp_id]) less or equal to search term (mp)
        bool is_min_lte = mps[mp_id] <= mp;
        if ((is_min_lte && !min_assigned) ||
            (min_assigned && is_min_lte && mps[mp_id] >= datas[i_min].mole_percents()[mp_id]))
        {
            i_min = i;
            min_assigned = true;
        }
    }
    return {i_min, i_max};
}

// specific heat
double Default_Data_Store::cp(Id id, Id data_id, double t, double p) const
{
    const auto& d = compounds[id].data[data_id];
    double t2 = t * t;
    double t_2 = 1.0/t2;

    // cp(t) = a + b * T + c * T^-2 + d * T^2
    return d.cp(t);
}
double Default_Data_Store::cp_h(Id id, Id data_id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data[data_id];
    return d.cp_h(enthalpy);
}

// viscosity
double Default_Data_Store::mu(Id id, Id data_id, double t, double p) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mu(t);
}
double Default_Data_Store::mu_h(Id id, Id data_id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mu_h(enthalpy);
}

// conductivity
double Default_Data_Store::k(Id id, Id data_id, double t, double p) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.k(t);
}
double Default_Data_Store::k_h(Id id, Id data_id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data[data_id];
    return d.k_h(enthalpy);
}

// density
double Default_Data_Store::rho(Id id, Id data_id, double t, double p) const
{
    const auto& d = compounds[id].data[data_id];
    return d.rho(t);
}
double Default_Data_Store::rho_h(Id id, Id data_id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data[data_id];
    return d.rho_h(enthalpy);
}

// enthalpy
double Default_Data_Store::h_t(Id id, Id data_id, double temperature) const
{
    const auto& d = compounds[id].data[data_id];
    return d.h_t(temperature);
}

// temperature
double Default_Data_Store::t_h(Id id, Id data_id, double enthalpy) const
{
    const auto& d = compounds[id].data[data_id];
    return d.h_to_t(enthalpy);
}

// melting temperature
double Default_Data_Store::melt(Id id) const
{
    const auto& d = compounds[id].data.front();
    return d.melt();
}

// boiling temperature
double Default_Data_Store::boil(Id id) const
{
    const auto& d = compounds[id].data.front();
    return d.boil();
}

void Default_Data_Store::Data::to_stream(std::ostream& s) const
{
    s << "% [";
    for (double pt : m_mole_percents) s << pt << " ";
    s << std::endl;
    s << "melt, boil: " << m_melt << ", " << m_boil << std::endl;
    s << "density :" << m_rho_a << ", " << m_rho_b << std::endl;

    s << "viscosity :" << m_mu_a << ", " << m_mu_b << std::endl;
    s << "conductivity :" << m_k_a << ", " << m_k_b << std::endl;
    s << "specific heat :" << m_cp_a << ", " << m_cp_b << ", " << m_cp_c << ", " << m_cp_d << std::endl;

}
} // namespace saline
