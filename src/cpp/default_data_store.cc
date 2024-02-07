#include "default_data_store.hh"
#include "saline_bug.hh"
#include "utils.hh"

#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace saline
{
    //@{
    //! Types
    using Id       = std::size_t;
    using Name     = std::string;
    using Vec_Id   = std::vector<Id>;
    using Vec_Name = std::vector<Name>;
    using Vec_Mole = std::vector<double>;
    //@}

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
void Default_Data_Store::load(const std::string& fPath)
{
    std::ifstream inFile(fPath.data());
    if(inFile.is_open())
    {
        load(inFile);
    }else{
        throw std::runtime_error("Falied to open input file");
    }
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
    // First two lines are header information not useful here
    std::getline(inFile,line);
    std::getline(inFile,line);
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

        // trim leading and trailing white space from all tokens
        for(size_t i=0; i<tokens.size(); ++i)
        {
            utils::trim(tokens[i]);
        }

        //Get the Data instance to fill in
        Default_Data_Store::Data& d = getDataReference(tokens[0],tokens[3]);

        //Molecular Weight
        parse_data_token(tokens[2],d.m_mole_weight);

        //Melt
        parse_data_qualifier(tokens[4],d.m_melt_qualifier);
        parse_data_token(tokens[4],d.m_melt);
        //Melt uncertainty
        parse_data_qualifier(tokens[5],d.m_melt_unc_qualifier);
        parse_data_token(tokens[5],d.m_melt_unc);
        d.m_melt_unc /= 100.0;
        //Melt Reference
        d.m_melt_ref = tokens[6];

        //Boil
        parse_data_qualifier(tokens[7],d.m_boil_qualifier);
        parse_data_token(tokens[7],d.m_boil);
        //Boil Uncertainty
        parse_data_qualifier(tokens[8],d.m_boil_unc_qualifier);
        parse_data_token(tokens[8],d.m_boil_unc);
        d.m_boil_unc /= 100.0;
        //Boil Reference
        d.m_boil_ref = tokens[9];

        //Density A and B as parameters for A - BT
        parse_data_token(tokens[10],d.m_rho_a);
        parse_data_token(tokens[11],d.m_rho_b);

        //Applicability range
        parse_range_token(tokens[12],d.m_rho_rng);
        //uncertainty in percent
        parse_data_qualifier(tokens[13],d.m_rho_unc_qualifier);
        parse_data_token(tokens[13],d.m_rho_unc);
        d.m_rho_unc /= 100.0;
        //reference
        d.m_rho_ref = tokens[14];

        //Viscosity A and B as parameters A*exp(B/(RT)) ... OR
        parse_data_token(tokens[15],d.m_mu_a);
        parse_data_token(tokens[16],d.m_mu_b);
        d.m_mu_b /= mGas_const;
        d.m_mu_c = std::numeric_limits<double>::quiet_NaN();
        //Test if we received inputs for two parameter viscosity model
        if (d.m_mu_a == 0.0)
        {
            // Since both a and b are zero try the other model
            // A, B, C for 10^(A + B/T + C/T**2)
            parse_data_token(tokens[17],d.m_mu_a);
            parse_data_token(tokens[18],d.m_mu_b);
            parse_data_token(tokens[19],d.m_mu_c);
        }
        //Applicability range
        parse_range_token(tokens[20],d.m_mu_rng);
        //uncertainty
        parse_data_qualifier(tokens[21],d.m_mu_unc_qualifier);
        parse_data_token(tokens[21],d.m_mu_unc);
        d.m_mu_unc /= 100.0;
        //reference
        d.m_mu_ref = tokens[22];

        //Thermal conductivity A and B as parameters for A + BT
        parse_data_token(tokens[23],d.m_k_a);
        parse_data_token(tokens[24],d.m_k_b);
        //Applicability range
        parse_range_token(tokens[25],d.m_k_rng);
        //uncertainty
        parse_data_qualifier(tokens[26],d.m_k_unc_qualifier);
        parse_data_token(tokens[26],d.m_k_unc);
        d.m_k_unc /= 100.0;
        //reference
        d.m_k_ref = tokens[27];

        //heat capacity A, B, C, D for A + B*T(K) + C*T-2(K) + D*T2(K)
        parse_data_token(tokens[28],d.m_cp_a);
        parse_data_token(tokens[29],d.m_cp_b);
        parse_data_token(tokens[30],d.m_cp_c);
        parse_data_token(tokens[31],d.m_cp_d);
        //Applicability range --  Not included in data

        //uncertainty
        parse_data_qualifier(tokens[32],d.m_cp_unc_qualifier);
        parse_data_token(tokens[32],d.m_cp_unc);
        d.m_cp_unc /= 100.0;
        //reference
        d.m_cp_ref = tokens[33];

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
    Id id = names_to_id(names);
    Data_Store::View m_impl;
    if( valid(id) )
    {
        // This uses a nearest neighbor search to set the composition
        m_impl = view(id);
        m_impl.assign_record(mole_percents);
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
    auto datas = compounds[id].data;
    for(size_t i =0; i < datas.size(); i++)
    {
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
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_rho(Id id, Id data_id) const
{
  return compounds[id].data[data_id].valid_rho();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_mu(Id id, Id data_id) const
{
  return compounds[id].data[data_id].valid_mu();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_k(Id id, Id data_id) const
{
  return compounds[id].data[data_id].valid_k();
}
//---------------------------------------------------------------------------//
/*!
 * \brief returns whether or not the selected data is valid
 */
bool Default_Data_Store::valid_cp(Id id, Id data_id) const
{
  return compounds[id].data[data_id].valid_cp();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on temperature
 */
double Default_Data_Store::cp(Id id, Id data_id, double t, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];

    // cp(t) = a + b * T + c * T^-2 + d * T^2
    return d.cp(t);
}
double Default_Data_Store::cp_h(Id id, Id data_id, double enthalpy, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];
    return d.cp_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity experimental range for the selected compound
 */
std::pair<double,double> Default_Data_Store::cp_rng(Id id, Id data_id) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.m_cp_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity uncertainty for the selected compound
 */
double Default_Data_Store::cp_unc(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.cp_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity reference for the selected compound
 */
std::string Default_Data_Store::cp_ref(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.cp_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on temperature
 */
double Default_Data_Store::mu(Id id, Id data_id, double t, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mu(t);
}
double Default_Data_Store::mu_h(Id id, Id data_id, double enthalpy, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mu_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity experimental range for the selected compound
 */
std::pair<double,double> Default_Data_Store::mu_rng(Id id, Id data_id) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.m_mu_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity uncertainty for the selected compound
 */
double Default_Data_Store::mu_unc(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mu_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity reference for the selected compound
 */
std::string Default_Data_Store::mu_ref(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mu_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on temperature
 */
double Default_Data_Store::k(Id id, Id data_id, double t, double /* p */) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.k(t);
}
double Default_Data_Store::k_h(Id id, Id data_id, double enthalpy, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];
    return d.k_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected compound
 */
std::pair<double,double> Default_Data_Store::k_rng(Id id, Id data_id) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.m_k_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity uncertainty for the selected compound
 */
double Default_Data_Store::k_unc(Id id, Id data_id) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.k_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity reference for the selected compound
 */
std::string Default_Data_Store::k_ref(Id id, Id data_id) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.k_ref();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on temperature
 */
double Default_Data_Store::rho(Id id, Id data_id, double t, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];
    return d.rho(t);
}
double Default_Data_Store::rho_h(Id id, Id data_id, double enthalpy, double /* p */) const
{
    const auto& d = compounds[id].data[data_id];
    return d.rho_h(enthalpy);
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity experimental range for the selected compound
 */
std::pair<double,double> Default_Data_Store::rho_rng(Id id, Id data_id) const
{
       const auto& d = compounds[id].data[data_id];
       // k(t) = a + b * t
       return d.m_rho_rng;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density uncertainty for the selected compound
 */
double Default_Data_Store::rho_unc(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.rho_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density reference for the selected compound
 */
std::string Default_Data_Store::rho_ref(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.rho_ref();
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
 * \brief retrieve the uncertainty in melting temperature of the selected compound
 */
double Default_Data_Store::melt_unc(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.melt_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the melting temperature reference of the selected compound
 */
std::string Default_Data_Store::melt_ref(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.melt_ref();
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
 * \brief retrieve the uncertainty in boiling temperature of the selected compound
 */
double Default_Data_Store::boil_unc(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.boil_unc();
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the boiling temperature reference of the selected compound
 */
std::string Default_Data_Store::boil_ref(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.boil_ref();
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
 * \brief retrieves the mole percent
 */
const Default_Data_Store::Vec_Mole& Default_Data_Store::mole_percent(Id id, Id data_id) const
{
    const auto& d = compounds[id].data[data_id];
    return d.mole_percents();
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

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given single component compound
 */
Default_Data_Store::Id Default_Data_Store::name_to_id(Name& name) const
{
    return names_to_id({name});
}

//---------------------------------------------------------------------------//
/*!
 * \brief obtain the data store id for the given multi-component compound
 */
Default_Data_Store::Id Default_Data_Store::names_to_id(Vec_Name in_names) const
{

    for (size_t i = 0; i < size(); ++i)
    {
        if (in_names == names(i)) return i;
    }
    return std::numeric_limits<std::size_t>::max();
}

Default_Data_Store::Data& Default_Data_Store::getDataReference(std::string names, std::string fracs)
{
  // Retrieve the chemical symbols from the salt description
  auto symbols = utils::split("-",names);

  //mole fraction of each salt
  Vec_Mole molfrac;
  if( symbols.size() == 1)
  {
      //This is for pure salt and might benefit from some error checking
      molfrac.push_back(1.0);
  }
  else
  {
      // Parse out the mole fractions
      for(auto mp : utils::split("-",fracs))
      {
          molfrac.push_back(std::stod(mp));
      }
      //Sort symbols alphabetically arranged mole fractions as needed
      auto sp = utils::getSortPermutation(symbols);
      utils::applySortPermuation_inPlace(symbols,sp);
      utils::applySortPermuation_inPlace(molfrac,sp);
  }

  // Attempt to find the salt in the existing data store
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
        
  return d;
}

void Default_Data_Store::parse_data_token(std::string &token,double &val)
{
  if (all_of(token.begin(),token.end(),[] (char c){ return c == '-';}))
  {
    val = 0.0;
  }
  else if(token == "synthetic")
  {
    val = 0.0;
  }else{
    // sometimes data comes packaged with a less than or greater than
    if (token[0] == '<') token.erase(0,1);
    if (token[0] == '>') token.erase(0,1);
    val = std::stod(token);
  }
}

void Default_Data_Store::parse_range_token(std::string &token,std::pair<double,double> &val)
{
  double stt = 0.0;
  double stp = 0.0;
  if (!all_of(token.begin(),token.end(),[] (char c){ return c == '-';}))
  {
    Vec_Name rng_str = utils::split("-",token);
    stt = std::stod(rng_str[0]);
    if (rng_str.size() == 2)
    {
      stp = std::stod(rng_str[1]);
    }
  }
  val = std::make_pair(stt,stp);
}

void Default_Data_Store::parse_data_qualifier(std::string &token,DataQualifier &qualifier)
{
  qualifier = NONE;
  // List of possible data qualifiers
  // TODO make legend style implementation
  //static const std::list<std::string> qualifiers_list() {return {"^","s","LG","*"};}
  switch(*token.end())
  {
    case '^':
      qualifier = PRESSURIZED;
      token.pop_back();
      break;
    case 's':
      qualifier = SUBLIMATES;
      token.pop_back();
      break;
    case 'G':// this actually represents "LG"
      qualifier = LIQUIDGAS;
      token.pop_back();
      token.pop_back();
      break;
    case '*':
      qualifier = PRESSURIZED;
      token.pop_back();
      break;
    default :
      break;
  }
}

Vec_Name Default_Data_Store::getSaltKeys() const
{
  Vec_Name keys;
  for(auto comp:compounds)
  {
    std::string key = comp.names[0];
    for(size_t i=1; i<comp.names.size(); ++i)
    {
      key.append("-");
      key.append(comp.names[i]);
    }
    keys.push_back(key);
  }
  return keys;
}

std::vector<std::vector<double>> Default_Data_Store::getSaltComps(Vec_Name names) const
{
  size_t id = names_to_id(names);
  std::vector<std::vector<double>> comps;
  if(valid(id))
  {
    const auto& d = compounds[id];
    for(size_t i=0; i<d.data.size(); ++i)
    {
      Vec_Mole mp = d.data[i].mole_percents();
      comps.push_back(mp);
    }
  }
  return comps;
}
} // namespace saline
