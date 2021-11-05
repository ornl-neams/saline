#include "r_kister_data_store.hh"
#include "default_data_store.hh"
#include "data_store.hh"
#include "saline_bug.hh"
#include "utils.hh"
#include "default_data.hh"

#include <stdexcept>
#include <algorithm>
#include <fstream>  
#include <sstream>  

//TODO all the enthalpy functions. They rely on setting up enthalpy tables. This
//is done for all the end members, but its not clear that carries much meaning
//for compositions of end members. We certainly shouldn't set up enthalpy tables
//for every possible combination of end members and we don't even know which
//tables would be used or when.

namespace saline
{
//----------------------------------------------------------------------------//
/*!
 * \brief constructs the Redlich-Kister data store extension
 */
R_Kister_Data_Store::R_Kister_Data_Store()
{
}

void R_Kister_Data_Store::load()
{
    d = Default_Data_Store();
    std::istringstream in(default_data_rk);
    d.load(in);
    load(in);
}
//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data into data store
 */
void R_Kister_Data_Store::load(const std::string& fPath)
{
    d = Default_Data_Store();
    std::ifstream inFile(fPath.data());
    d.load(inFile);
    load(inFile);
}
//---------------------------------------------------------------------------//
/*!
 * \brief helper function to load data Redlich-Kister parameters into data store
 */
void R_Kister_Data_Store::load(std::istream& inFile)
{
    // Set up space for the mixing models
    int num_binaries = d.size();
    m_rho.resize(num_binaries,std::vector<R_Kister_Data_Store::RK_Polynomial>(num_binaries));

    ////TODO no input for these, implemented only for downstream process
    m_mu.resize(num_binaries,std::vector<R_Kister_Data_Store::RK_Polynomial>(num_binaries));
    m_cp.resize(num_binaries,std::vector<R_Kister_Data_Store::RK_Polynomial>(num_binaries));
    m_k.resize(num_binaries,std::vector<R_Kister_Data_Store::RK_Polynomial>(num_binaries));

    // Jaunt through lines until we find the Redlich-Kister parameters
    std::string line;
    while( std::getline(inFile,line) )
    {
        if( line.find("RK parameters") != std::string::npos ) break;
    }
    // Its possible we could have a bum input. That should break here.

    // this is  a comment line
    std::getline(inFile,line);

    // Read the input data. TODO currently only uses density
    while( std::getline(inFile,line))
    {
        auto tokens = utils::split(",",line);
        for(size_t i=0; i<tokens.size(); ++i)
        {
            utils::trim(tokens[i]);
        }

        //Sort components and adjust value if required
        //L1_a, L1_b, L2_a, L2_b, L3_a, L3_b, L4_a, L4_b
        size_t id_a;
        size_t id_b;
        double sortFactor;
        if(tokens[0] < tokens[1])
        {
          id_a = d.names_to_id({tokens[0]});
          id_b = d.names_to_id({tokens[1]});
          sortFactor = 1.0;
        }
          else
        {
          id_a = d.names_to_id({tokens[1]});
          id_b = d.names_to_id({tokens[0]});
          sortFactor = -1.0;
        }

        //component a
        m_rho[id_a][id_b].a_n.resize(4);
        m_rho[id_a][id_b].a_n[0] = std::stod(tokens[2]);
        m_rho[id_a][id_b].a_n[1] = sortFactor * std::stod(tokens[4]);
        m_rho[id_a][id_b].a_n[2] = std::stod(tokens[6]);
        //componet b
        m_rho[id_a][id_b].b_n.resize(4);
        m_rho[id_a][id_b].b_n[0] = std::stod(tokens[3]);
        m_rho[id_a][id_b].b_n[1] = sortFactor * std::stod(tokens[5]);
        m_rho[id_a][id_b].b_n[2] = std::stod(tokens[7]);

        //T_min, T_max
        m_rho[id_a][id_b].t_min = std::stod(tokens[8]);
        m_rho[id_a][id_b].t_max = std::stod(tokens[9]);

        //algorithm
        //reference
    }
}
//----------------------------------------------------------------------------//
/*!
 * \brief finds and sets the required information for modeling a requested salt
 */
Data_Store::View R_Kister_Data_Store::setView( const Vec_Name& names, const Vec_Mole& mole_percents)
{
    end_members.clear();
    end_members.resize(names.size());

    //Get the permutation to sort names alphabetically
    auto sp = utils::getSortPermutation(names);
    //Use that to sort the names and mole percents
    auto sort_names = utils::applySortPermuation(names,sp);
    endMem_moleFracs = utils::applySortPermuation(mole_percents,sp);;

    // For the functioning of this data store...views of end_members are stored
    for(size_t i=0; i<names.size(); ++i)
    {
        //TODO Assuming they are all valid single components for now.
        //Get the ids of the sorted names...so all the ids are in order
        end_members[i] = (d.setView({sort_names[i]},{1.0}));
    }

    // To adapt the interfaces a "minimal" view is returned to the client
    Data_Store::View v;

    if(std::all_of(end_members.begin(),end_members.end(),[] 
          (Data_Store::View v){return !v.null();}))
    {
      v = view(0);
      v.mole_percents = mole_percents;
    }
    return v;
}

//----------------------------------------------------------------------------//
/*!
 * \brief returns the names of the compound currently being investigated
 */
std::vector<std::string> R_Kister_Data_Store::names(Id id) const
{
    std::vector<std::string> comp_names;
    return comp_names;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on temperature
 */
double R_Kister_Data_Store::cp( Id id, Id data_id, double temperature, double pressure) const
{
    //Ideal mixing
    double cp_ideal = 0.0;
    for( int i=0; i<end_members.size(); ++i)
    {
        cp_ideal += end_members[i].cp(temperature) * endMem_moleFracs[i];
    }

    //Excess
    double cp_excess = 0.0;
    for(size_t j=0; j<end_members.size()-1;++j)
    {
        int j_id = end_members[j].id;
        for(size_t i=j+1; i<end_members.size();++i)
        {
            int i_id = end_members[i].id;
            RK_Polynomial excess_calc = m_cp[j_id][i_id];
            cp_excess += excess_calc.getRK_solution(endMem_moleFracs[j],endMem_moleFracs[i],temperature);
        }
    }
    return cp_ideal + cp_excess;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the heat capacity for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::cp_h(Id id, Id data_id, double enthalpy, double pressure) const
{
    //TODO waiting on enthalpy calculation flow
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on temperature
 */
double R_Kister_Data_Store::mu(Id id, Id data_id, double temperature, double pressure) const
{
    //Ideal mixing
    double mu_ideal = 0.0;
    for( int i=0; i<end_members.size(); ++i)
    {
        mu_ideal += end_members[i].mu(temperature) * endMem_moleFracs[i];
    }

    //Excess
    double mu_excess = 0.0;
    for(size_t j=0; j<end_members.size()-1;++j)
    {
        int j_id = end_members[j].id;
        for(size_t i=j+1; i<end_members.size();++i)
        {
            int i_id = end_members[i].id;
            RK_Polynomial excess_calc = m_mu[j_id][i_id];
            mu_excess += excess_calc.getRK_solution(endMem_moleFracs[j],endMem_moleFracs[i],temperature);
        }
    }
    return mu_ideal + mu_excess;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the viscosity for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::mu_h(Id id, Id data_id, double enthalpy, double pressure) const
{
    //TODO waiting on enthalpy calculation flow
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on temperature
 */
double R_Kister_Data_Store::k(Id id, Id data_id, double temperature, double pressure) const
{
    //Ideal mixing
    double k_ideal = 0.0;
    for( int i=0; i<end_members.size(); ++i)
    {
        k_ideal += end_members[i].k(temperature) * endMem_moleFracs[i];
    }

    //Excess
    double k_excess = 0.0;
    for(size_t j=0; j<end_members.size()-1;++j)
    {
        int j_id = end_members[j].id;
        for(size_t i=j+1; i<end_members.size();++i)
        {
            int i_id = end_members[i].id;
            RK_Polynomial excess_calc = m_k[j_id][i_id];
            k_excess += excess_calc.getRK_solution(endMem_moleFracs[j],endMem_moleFracs[i],temperature);
        }
    }
    return k_ideal + k_excess;
}
//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the conductivity for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::k_h(Id id, Id data_id, double enthalpy, double pressure) const
{
    //TODO waiting on enthalpy calculation flow
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on temperature
 */
double R_Kister_Data_Store::rho(Id id, Id data_id, double temperature, double pressure) const
{
    // Only one end member means that density is equal to its density
    if(end_members.size() == 1) return end_members[0].rho(temperature);

    //Ideal component
    double m_t = 0.0;
    double v_t = 0.0;
    for(size_t i=0; i<end_members.size();++i)
    {
        //Component molecular weight contribution
        double endMass = endMem_moleFracs[i]*end_members[i].molecularWeight();
        //Sum of molecular weights contributions
        m_t += endMass;
        //Sum of volume fraction contributions
        v_t += endMass/end_members[i].rho(temperature);
    }
    double rho_ideal = m_t/v_t;

    //Excess component
    double rho_excess = 0.0;
    for(size_t j=0; j<end_members.size()-1;++j)
    {
        int j_id = end_members[j].id;
        for(size_t i=j+1; i<end_members.size();++i)
        {
            int i_id = end_members[i].id;
            RK_Polynomial excess_calc = m_rho[j_id][i_id];
            rho_excess += excess_calc.getRK_solution(endMem_moleFracs[j],endMem_moleFracs[i],temperature);
        }
    }
    return rho_ideal + rho_excess;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the density for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::rho_h(Id id, Id data_id, double enthalpy, double pressure) const
{
    //Ideal mixing
    double h_ideal = 0.0;

    //Excess
    double h_excess = 0.0;
    return h_ideal + h_excess;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the enthalpy for the selected compound based on temperature
 */
// enthalpy
double R_Kister_Data_Store::h_t(Id id, Id data_id, double temperature) const
{
    //TODO waiting on enthalpy calculation flow
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the temperature for the selected compound based on enthalpy
 */
double R_Kister_Data_Store::t_h(Id id, Id data_id, double enthalpy) const
{
    //TODO waiting on enthalpy calculation flow
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the melting temperature of the selected compound
 */
double R_Kister_Data_Store::melt(Id id, Id data_id) const
{
    //TODO I am not sure what we are doing here. Deferring until I have an answer
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieve the boiling temperature of the selected compound
 */
double R_Kister_Data_Store::boil(Id id, Id data_id) const
{
    //TODO I am not sure what we are doing here. Deferring until I have an answer
    double cp = 0.0;
    return cp;
}

//----------------------------------------------------------------------------//
/*!
 * \brief return the number of compounds stored in this data store
 */
std::size_t R_Kister_Data_Store::constituent_count(Id id) const
{
    //TODO It isn't immediately clear what this function should return.
    //Is the constituent count the number of end members, binaries, ternaries?
    return id;
}

//----------------------------------------------------------------------------//
/*!
 * \brief returns the ID of the nearest matching compound composition
 */
std::size_t R_Kister_Data_Store::nearest(Id id, const Vec_Mole& mole_percent) const
{
    //TODO This doesn't actually have a meaning at all
    return id;
}

//----------------------------------------------------------------------------//
/*!
 * \brief retrieves the molecular weight for the provided salt
 */
double R_Kister_Data_Store::molecularWeight(Id id, Id data_id) const
{
    double molWt = 0.0;
    for(size_t i=0; i<end_members.size();++i)
    {
        molWt += endMem_moleFracs[i]* end_members[i].molecularWeight();
    }
    return molWt;
}

//----------------------------------------------------------------------------//
/*!
 * \brief Calculates the solution to a Redlich-Kister polynomial
 */
double R_Kister_Data_Store::RK_Polynomial::getRK_solution(double x, double y, double temperature)
{
    // Without any fit coefficients the result should be 0.0
    if(a_n.size() < 1) return 0.0;

    // Construct the fit terms
    std::vector<double> l_n(a_n.size());
    for(int i=0; i<l_n.size(); ++i)
    {
        l_n[i] = a_n[i] + (b_n[i] * temperature);
    }

    // difference of weights used to control the fit
    double w_diff = x - y;

    //First term is unweighted
    double summation = l_n[0];
    for(int i=1; i<l_n.size(); ++i)
    {
        summation += l_n[i]*std::pow(w_diff,i);
    }

    return (x*y)*summation;
}

} // namespace saline
