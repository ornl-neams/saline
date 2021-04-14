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
    std::getline(inFile,line);
    while (std::getline(inFile,line))
    {
        utils::trim(line);
        //For now if an empty line signals the end of data
        if( line.empty() ) break;
        //Skip any comments
        if( line[0] == '/' && line[1] == '/' ) continue;

        // names, id, mole_percents, tmelt, tboil, rho_a,  rho_b, mu_a, mu_b, k_a, k_b, cp_a, cp_b, cp_c, cp_d
        auto tokens = utils::split(",",line);
        for(size_t i=0; i<tokens.size(); ++i)
        {
            utils::trim(tokens[i]);
        }

        //Chemical symbols
        auto symbols = utils::split("-",tokens[0]);

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
            for(auto mp : utils::split("-",tokens[1]))
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

        //Molecular Weight
        d.m_mole_weight = std::stod(tokens[2]);
        //Melt
        d.m_melt = std::stod(tokens[3]);
        //Melt uncertainty
        d.m_melt_unc = std::stod(tokens[4]);
        //Melt Reference

        //Boil
        d.m_boil = std::stod(tokens[6]);
        //Boil Uncertainty
        d.m_boil_unc = std::stod(tokens[7]);
        //Boil Reference

        //Density A and B as parameters for A - BT
        d.m_rho_a = std::stod(tokens[9]);
        d.m_rho_b = std::stod(tokens[10]);

        //Applicability range
        std::vector<std::string> rho_rng_str = utils::split("-",tokens[11]);
        d.m_rho_rng = std::make_pair(std::stod(rho_rng_str[0]),std::stod(rho_rng_str[1]));
        //uncertainty
        d.m_rho_unc = std::stod(tokens[12]);
        //reference

        //Viscosity A and B as parameters A*exp(B/(RT)) ... OR
        d.m_mu_a = std::stod(tokens[14]);
        d.m_mu_b = std::stod(tokens[15])/mGas_const;
        d.m_mu_c = std::numeric_limits<double>::quiet_NaN();
        //Test if we received inputs for two parameter viscosity model
        if( d.m_mu_a == 0.0 and d.m_mu_b == 0.0)
        {
            // Since both a and b are zero try the other model
            // A, B, C for 10^(A + B/T + C/T**2)
            d.m_mu_a = std::stod(tokens[16]);
            d.m_mu_b = std::stod(tokens[17]);
            d.m_mu_c = std::stod(tokens[18]);
        }
        //Applicability range
        std::vector<std::string> mu_rng_str = utils::split("-",tokens[19]);
        d.m_mu_rng = std::make_pair(std::stod(mu_rng_str[0]),std::stod(mu_rng_str[1]));
        //uncertainty
        d.m_mu_unc = std::stod(tokens[20]);
        //reference

        //Thermal conductivity A and B as parameters for A + BT
        d.m_k_a  = std::stod(tokens[22]);
        d.m_k_b  = std::stod(tokens[23]);
        //Applicability range
        std::vector<std::string> k_rng_str = utils::split("-",tokens[24]);
        d.m_k_rng = std::make_pair(std::stod(k_rng_str[0]),std::stod(k_rng_str[1]));
        //uncertainty
        d.m_k_unc = std::stod(tokens[25]);
        //reference

        //heat capacity A, B, C, D for A + B*T(K) + C*T-2(K) + D*T2(K)
        d.m_cp_a = std::stod(tokens[27]);
        d.m_cp_b = std::stod(tokens[28]);
        d.m_cp_c = std::stod(tokens[29]);
        d.m_cp_d = std::stod(tokens[30]);
        //Applicability range --  Not included in data

        //uncertainty
        d.m_cp_unc = std::stod(tokens[31]);
        //reference

    }

    setup_enthalpy_tables();
}

//----------------------------------------------------------------------------//
/*!
 * \brief setup the enthalpy 2 temperature interpolation tables
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief calculate the values of the enthalpy interpolation tables
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief helper function to calculate the enthalpy integration
 */
double Default_Data_Store::Data::h_t(double t) const
{
    double t2 = t * t;
    double t3 = t * t2;
    // h(t) = a * t + b/2 t^2 - c / t + d/3 * t^3 - E;
    return m_cp_a * t + m_cp_b / 2.0 * t2 - m_cp_c / t + m_cp_d / 3.0 * t3 - m_e;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the enthalpy for the selected compound based on temperature
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief returns the number of compound stored in this data store
 */
std::size_t Default_Data_Store::constituent_count(Id id) const
{
    saline_require(id < compounds.size());
    return compounds[id].names.size();
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


//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on temperature
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on temperature
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on temperature
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on temperature
 */
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

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the enthalpy for the selected compound based on temperature
 */
double Default_Data_Store::h_t(Id id, Id data_id, double temperature) const
{
    const auto& d = compounds[id].data[data_id];
    return d.h_t(temperature);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the temperature for the selected compound based on enthalpy
 */
double Default_Data_Store::t_h(Id id, Id data_id, double enthalpy) const
{
    const auto& d = compounds[id].data[data_id];
    return d.h_to_t(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the melting temperature of the selected compound
 */
double Default_Data_Store::melt(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.melt();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the boiling temperature of the selected compound
 */
double Default_Data_Store::boil(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.boil();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
double Default_Data_Store::molecularWeight(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.molecularWeight();
}

//----------------------------------------------------------------------------//
/*!
 *
 */
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
