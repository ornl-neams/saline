#include "../default_data_store.hh"
#include "../thermophysical_properties.hh"
#include "../utils.hh"

#include <iostream>
#include <iomanip>

// macro for printing error and 
#define exit_msg(X) \
      { std::cout << X << std::endl; return 1;} 

#define ASSERT_TRUE(C) \
      { if(!C) {std::cout << "FAILED: "<< #C << std::endl; return 1;} }  

#define ASSERT_EQ(E, A) \
      { if(E != A) {std::cout  << "FAILED: "<< #E <<" != " #A << std::endl \
                      << #E <<" = "<< std::setprecision(16)<< E  \
                      <<" and " << #A <<" = " << std::setprecision(16) << A << std::endl; return 1;} }  

using namespace saline;

int main()
{
    Default_Data_Store d;
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    ASSERT_TRUE(tp.setComposition({"BeCl2"},{1.0}));
    std::cout <<"BeCl2 ... "  <<std::endl;
    std::cout << "rho : "<< tp.rho(973.15) << " to " << tp.rho(2000) << std::endl;
    std::cout << "enthalpy : " << tp.h_t(973.15) << " to " << tp.h_t(2000) << std::endl;
    std::cout << "temperature : " << tp.t_h(tp.h_t(973.15)) << " to " << tp.t_h(tp.h_t(2000)) << std::endl;
    
    std::cout << "rho_h : " << tp.rho_h(0) << " to " << tp.rho_h(100) << std::endl;

    ASSERT_TRUE(tp.setComposition({"KCl","MgCl2"},{0.68,0.32}));
    std::cout <<"MgCl2-KCl..." << std::endl;

    // expecting ~0.47 and ~.43 + or - 3 W/m K
    std::cout << tp.mu(utils::c2k(450)) <<  " " << tp.mu(utils::c2k(800)) << std::endl;

    // expecting ~1680 and ~1490 + or - 25 kg/m^3
    std::cout << tp.rho(utils::c2k(450)) <<  " " << tp.rho(utils::c2k(800)) << std::endl;

    return 0;   
}