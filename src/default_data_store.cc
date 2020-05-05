#include "default_data_store.hh"

#include <algorithm>

namespace saline
{

void Default_Data_Store::add(const std::vector<std::string>& names, 
            const std::vector<double>& mole_percent, 
            double melt, double boil,
            double rho_a, double rho_b,
            double mu_a, double mu_b,
            double k_a, double k_b,
            double cp_a, double cp_b, double cp_c, double cp_d)
{
    if (compounds.empty())
    {
        compounds.resize(compounds.size()+1);
    }

    // if it is a new compound, add empty entry
    if( compounds.back().names != names)
    {
        compounds.resize(compounds.size()+1);
    }
    auto& c  = compounds.back();
    c.names = names;
    c.data.resize(c.data.size()+1);
    auto& d = c.data.back();
    d.m_mole_percents = mole_percent;
    d.m_melt = melt;
    d.m_boil = boil;    
    d.m_rho_a = rho_a;
    d.m_rho_b = rho_b;
    d.m_mu_a = mu_a;
    d.m_mu_b = mu_b;
    d.m_k_a  = k_a ;
    d.m_k_b  = k_b ;
    d.m_cp_a = cp_a;
    d.m_cp_b = cp_b;
    d.m_cp_c = cp_c;
    d.m_cp_d = cp_d;
}
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
    add({"BeCl2"},{1},688,755,2.276,0.0011,0.0,0.0,0.0,0.0,121.42,0.0,0.0,0.0);
    add({"BeF2"},{1},825,1442,1.972,0.0000145,3.0184E-07,239257.6748,0.7984,0.0,102.652,-0.001539,-15651800,0.000000003);
    add({"CaCl2"},{1},1045,2208.7,2.5261,0.0004225,0.10215,30350.99274,0.4468,0.0,102.533,0.0,0.0,0.0);
    add({"CaF2"},{1},1691,2806.6,3.179,0.000391,0.0,0.0,0.565,0.0,99.914,0.0,0.0,0.0);
    add({"GdCl3"},{1},882,1868,4.1484,0.0006707,0.0,0.0,0.0,0.0,139.33,0.0,0.0,0.0);
    add({"GdF3"},{1},1505,2550,0.0,0.0,0.0,0.0,0.0,0.0,127.82,0.0,0.0,0.0);
    add({"KCl"},{1},1044,1750,2.1359,0.0005831,0.06166,25047.2656,0.736,0.0,73.597,0.0,0.0,0.0);
    add({"KCl","MgCl2"},{0.68,0.32},697.55,0.0,2.0007,0.00045709,0.01408,2262.979,0.2469,0.0005025,0.0,0.0,0.0,0.0);
    add({"KF"},{1},1131,1775,2.6464,0.0006515,0.1058,23778.99031,0.5684,0.0,71.965,0.0,0.0,0.0);
    add({"LaCl3"},{1},1131,2020,4.0895,0.0007774,0.02061,54597.92943,0.0,0.0,157.74,0.0,0.0,0.0);
    add({"LaF3"},{1},1766,2600,5.793,0.000582,0.0,0.0,0.0,0.0,167.36,0.0,0.0,0.0);
    add({"LiCl"},{1},883,1656,1.8842,0.0004328,0.10852,19111.36734,0.7265,0.0,63.2,0.0,0.0,0.0);
    add({"LiCl","KCl "},{0.557,0.443},628,1673.15,2.0285,0.00052676,0.08703,20852.56807,0.0,0.0,70.93,0.0,0.0,0.0);
    add({"LiF"},{1},1121.3,1946,2.3581,0.0004902,0.18359,21832.01701,1.436,0.0,64.183,0.0,0.0,0.0);
    add({"LiF","BeF2"},{0.66,0.34},730.2,0.0,2.7637,0.000687,0.0116,3755,0.629697,0.0005,2385,0.0,0.0,0.0);
    add({"LiF","BeF2"},{0.69,0.31},778.15,0.0,0.0,0.0,0.118,3624,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","ThF4"},{0.7011,0.2388,0.0601},0.0,0.0,3.1118,0.0006707,0.06602,4380,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","ThF4"},{0.7006,0.1796,0.1198},0.0,0.0,3.8236,0.0008064,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","ThF4"},{0.6998,0.1499,0.1503},0.0,0.0,4.1811,0.0009526,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","ThF4"},{0.727,0.157,0.116},0.0,0.0,0.0,0.0,0.1094,4092,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","UF4","ThF4"},{0.71,0.16,0.01,0.12},0.0,0.0,0.0,0.0,0.062,4666,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","ZrF4"},{0.647,0.301,0.052},0.0,0.0,2.5387,0.0005769,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","BeF2","ZrF4","ThF4"},{0.6479,0.2996,0.0499,0.0026},0.0,0.0,2.5533,0.000562,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"LiF","NaF","BeF2"},{0.31,0.31,0.38},611,0.0,2.4335,0.00045,0.0000338,4738,0.7,0.0,2200,0.0,0.0,0.0);
    add({"LiF","NaF","KF"},{0.465,0.115,0.42},727,1843.15,2.5793,0.0006237,0.02487,4478.62,0.36,0.00056,976.78,1.0634,0.0,0.0);
    add({"LiF","RbF"},{0.43,0.57},748.15,0.0,0.0,0.0,0.0212,4678,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"MgCl2"},{1},987,1685,1.95,0.0002712,0.17939,20558.67899,0.204,0.0,92.048,0.000002,5921200,0.0);
    add({"MgF2"},{1},1536,2500,3.235,0.000524,0.0,0.0,0.6501,0.0,94.922,0.0,0.0,0.0);
    add({"NaCl"},{1},1073.8,1738,2.1389,0.0005426,0.089272,21960.0914,0.4923,0.0,67.9,0.0,0.0,0.0);
    add({"NaF"},{1},1269,1977,2.755,0.000636,0.1197,26468.42682,0.9026,0.0,70.1,0.0,0.0,0.0);
    add({"NaF","BeF2"},{0.57,0.43},633.15,0.0,0.0,0.0,0.0346,5164,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","KF","BeF2"},{0.49,0.15,0.36},828.15,0.0,0.0,0.0,0.0811,4008,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","KF","BeF2"},{0.49,0.15,0.36},828.15,0.0,0.0,0.0,0.0811,4008,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","KF","UF4"},{0.465,0.26,0.275},811.15,0.0,0.0,0.0,0.0767,4731,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","KF","ZrF4"},{0.05,0.52,0.43},698.15,0.0,0.0,0.0,0.161,3171,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF"},{0.40,0.60},925.15,0.0,0.0,0.0,0.116,3225,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","BeF2"},{0.56,0.16,0.28},751.15,0.0,0.0,0.0,0.111,3486,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","BeF2"},{0.64,0.05,0.31},828.15,0.0,0.0,0.0,0.138,3435,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","BeF2"},{0.53,0.24,0.23},808.15,0.0,0.0,0.0,0.157,3168,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","BeF2"},{0.49,0.36,0.15},870.15,0.0,0.0,0.0,0.173,3043,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","BeF2","UF4"},{0.56,0.21,0.2,0.03},778.15,0.0,0.0,0.0,0.0737,4012,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","KF","UF4"},{0.109,0.445,0.435,0.011},725.15,0.0,0.0,0.0,0.0348,4265,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","KF","UF4"},{0.112,0.453,0.41,0.025},763.15,0.0,0.0,0.0,0.0292,4507,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","ZrF4"},{0.22,0.55,0.23},843.15,0.0,0.0,0.0,0.585,4647,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","LiF","ZrF4","UF4"},{0.20,0.55,0.21,0.04},818.15,0.0,0.0,0.0,0.0585,4647,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","UF4"},{0.667,0.333},938.15,0.0,0.0,0.0,0.181,3927,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","ZrF4 "},{0.50,0.50},783.15,0.0,0.0,0.0,0.0709,4168,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","ZrF4","UF4"},{0.50,0.46,0.04},793.15,0.0,0.0,0.0,0.0981,3895,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","ZrF4","UF4"},{0.535,0.40,0.065},813.15,0.0,0.0,0.0,0.194,3302,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NaF","ZrF4","UF4"},{0.56,0.39,0.05},803.15,0.0,0.0,0.0,0.104,3798,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NdCl3"},{1},1032,1873,4.2642,0.00093014,0.0,0.0,0.0,0.0,146.44,0.0,0.0,0.0);
    add({"NdF3"},{1},1650,2573,0.0,0.0,0.0,0.0,0.0,0.0,184.473,-0.004473,-11924400,0.0);
    add({"NpCl3"},{1},1073,1800,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"NpF3"},{1},1698,2500,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"PuCl3"},{1},1033,2063,0.0,0.0,0.0,0.0,0.0,0.0,133.888,0.0,0.0,0.0);
    add({"PuF3"},{1},1699,2493,0.0,0.0,0.0,0.0,0.0,0.0,146.44,0.0,0.0,0.0);
    add({"RbF","ZrF4","UF4"},{0.48,0.48,0.04},698.15,0.0,0.0,0.0,0.116,3590,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"RbF","ZrF4","UF4"},{0.50,0.46,0.04},773.15,0.0,0.0,0.0,0.0657,4081,0.0,0.0,0.0,0.0,0.0,0.0);
    add({"SrCl2"},{1},1147,1523,3.3896,0.0005781,0.09638,34917.82146,0.3802,0.0,104.6,0.0,0.0,0.0);
    add({"SrF2"},{1},1750,2733,4.784,0.0005781,0.0,0.0,0.3954,0.0,99.048,0.0,0.0,0.0);
    add({"UCl3"},{1},1110,2000,13.652,0.0013017,0.0,0.0,0.0,0.0,129.704,0.0,0.0,0.0);
    add({"UF3"},{1},1700,2550,0.0,0.0,0.0,0.0,0.0,0.0,134,0.0,0.0,0.0);
    add({"UF4"},{1},1309,1690,7.784,0.000992,0.010775,58222.17086,0.0,0.0,167,0.0,0.0,0.0);

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
    m_h_t.resize(table_size);
    m_dt = (m_boil - m_melt) / m_h_t.size();
    
    double melt2 = m_melt * m_melt;
    double melt3 = m_melt * melt2;
    // E = a * melt + b * melt^2  + d * melt^3 - c / melt;
    double m_e = m_cp_a * m_melt + m_cp_b * melt2 + m_cp_d * melt3 - m_cp_c / m_melt;
    // h(melt) = 0
    m_h_t[0] = 0.0;
    size_t i = 1;
    for (double t = m_melt + m_dt; t < m_boil; t+=m_dt, ++i)
    {        
        m_h_t[i] = h_t(t);
    }
}

double Default_Data_Store::Data::h_t(double t) const
{

    double t2 = t * t;
    double t3 = t * t3;
    // h(t) = a * t + b t^2 - c / t + d * t^3 - E;
    return m_cp_a * t + m_cp_b * t2 - m_cp_c / t + m_cp_d * t3 - m_e;
}

double Default_Data_Store::Data::h_to_t(double h) const
{
    if (m_h_t.empty()) return std::numeric_limits<double>::quiet_NaN();

    if (h < 0) return m_h_t.front();

    auto x = std::lower_bound(m_h_t.begin(), m_h_t.end(), h);
    if (x == m_h_t.end() ) return m_h_t[m_h_t.size() - 1];

    double y0 = std::distance(m_h_t.begin(), x) * m_dt;
    double y1 = y0 + m_dt;
    double x0 = *x; x++;
    double x1 = *x;

    return y0 + (h-x0) * (y1-y0) / (x1-x0);
}

// specific heat 
double Default_Data_Store::cp(Id id, double t, double p) const
{
    const auto& d = compounds[id].data.front();
    double t2 = t * t;
    double t_2 = 1.0/t2;

    // cp(t) = a + b * T + c * T^-2 + d * T^2
    return d.cp(t);
}
double Default_Data_Store::cp_h(Id id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data.front();
    return d.cp_h(enthalpy);
}

// viscosity
double Default_Data_Store::mu(Id id, double t, double p) const
{
    const auto& d = compounds[id].data.front();    
    return d.mu(t);
}
double Default_Data_Store::mu_h(Id id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data.front();
    return d.mu_h(enthalpy);
}

// conductivity
double Default_Data_Store::k(Id id, double t, double p) const
{
       const auto& d = compounds[id].data.front();
       // k(t) = a + b * t
       return d.k(t);
}
double Default_Data_Store::k_h(Id id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data.front();
    return d.k_h(enthalpy);
}

// density
double Default_Data_Store::rho(Id id, double t, double p) const
{
    const auto& d = compounds[id].data.front();    
    return d.rho(t);
}
double Default_Data_Store::rho_h(Id id, double enthalpy, double p) const
{
    const auto& d = compounds[id].data.front();
    return d.rho_h(enthalpy);
}

// enthalpy
double Default_Data_Store::h_t(Id id, double temperature) const
{
    const auto& d = compounds[id].data.front();
    return d.h_t(temperature);
}

// temperature
double Default_Data_Store::t_h(Id id, double enthalpy) const
{
    const auto& d = compounds[id].data.front();
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
} // namespace saline