#include "../default_data_store.hh"
#include "../thermophysical_properties.hh"

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

    std::cout << "rho : "<< tp.rho(973.15) << " to " << tp.rho(2000) << std::endl;

    ASSERT_EQ(1.205535, tp.rho(973.15));
    ASSERT_EQ(0.076, tp.rho(2000));
    return 0;   
}