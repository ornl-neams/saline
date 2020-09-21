#include "gtest/gtest.h"

#include "../default_data_store.hh"
#include "../thermophysical_properties.hh"
#include "../utils.hh"

#include <iostream>

using namespace saline;

TEST(default_data, rho_LiFBeF2ThF4_7011_2388_0601)
{
    // measured t(c) for LiF-BeF2-ThF4 @ 70.11, 23.88, 6.01 mole %) Cantor '73 (https://www.osti.gov/servlets/purl/4419855)
    std::vector<double> tcs = {555.1,571.9, 580.9, 596.7, 606.2, 621.3, 628.7, 646.8, 655.1, 673.2, 681.2, 707.4};
    // Experimental values
    std::vector<double> rho_exp = {2.74, 2.727, 2.723, 2.711, 2.704, 2.694, 2.690, 2.677, 2.671, 2.660, 2.653, 2.639};
    // Expected calculated values
    std::vector<double> rho_calc_ref = {2.7395, 2.7282, 2.7222, 2.7116, 2.7052, 2.6951, 2.6901, 2.6780, 2.6724, 2.6603, 2.6549, 2.6372}; 

    Default_Data_Store d;
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));
    
    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.7011,0.2388,0.0601}));

    std::cout << "Density:" << std::endl
              << " T(c) experimental calc_ref calced" << std::endl;
    
    for( size_t i = 0; i < tcs.size(); ++i)
    {
        double t_c = tcs[i];        
        // NOTE: the data for cantor is reported in C but the interface requires K
        // for now pass C
        EXPECT_NEAR(rho_calc_ref[i], tp.rho(t_c), 1e-3);
        // print data
        std::cout << t_c << " " << rho_exp[i] 
                         << " " << rho_calc_ref[i] 
                         << " " << tp.rho(t_c) 
                         << " " << rho_calc_ref[i] - tp.rho(t_c) << std::endl;
    }
}

TEST(default_data, rho_LiFBeF2ThF4_6998_1499_1503)
{
    // measured t(c) for LiF-BeF2-ThF4 @ 69.98, 14.99, 15.03 mole %) Cantor '73 (https://www.osti.gov/servlets/purl/4419855)
    std::vector<double> tcs           = {543.4, 556.7, 582.9, 608.5, 620.9, 633.7, 646.4, 659.1, 672.6, 698.9, 713.7, 730.2, 749.5};
    // Experimental values
    std::vector<double> rho_exp       = {3.663, 3.649, 3.624, 3.602, 3.589, 3.578, 3.566, 3.554, 3.541, 3.516, 3.500, 3.483, 3.466};
    // Expected calculated values
    std::vector<double> rho_calc_ref  = {3.6634, 3.6507, 3.6258, 3.6014, 3.5896, 3.5774, 3.5653, 3.5532, 3.5403, 3.5153, 3.5012, 3.4854, 3.4671};

    Default_Data_Store d;
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));
    
    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.6998,0.1499,0.1503}));

    std::cout << "Density:" << std::endl
              << " T(c) experimental calc_ref calced" << std::endl;
    
    for( size_t i = 0; i < tcs.size(); ++i)
    {
        double t_c = tcs[i];        
        // NOTE: the data for cantor is reported in C but the interface requires K
        // for now pass C
        EXPECT_NEAR(rho_calc_ref[i], tp.rho(t_c), 1e-3);
        // print data
        std::cout << t_c << " " << rho_exp[i] 
                         << " " << rho_calc_ref[i] 
                         << " " << tp.rho(t_c) 
                         << " " << rho_calc_ref[i] - tp.rho(t_c) << std::endl;
    }
}

TEST(default_data, mu_LiFBeF2ThF4_727_157_116)
{
    // measured t(c) for LiF-BeF2-ThF4 @ 72.7, 15.7, 11.6 mole %) Cantor '73 (https://www.osti.gov/servlets/purl/4419855)
    std::vector<double> tcs           = {553, 582, 613, 638, 622, 588, 555, 572, 649, 673};
    // Experimental values
    std::vector<double> mu_exp       = {14.3, 13.4, 11.4, 9.74, 11.5, 13.5, 16.5, 14.1, 9.79, 7.74};
    // Expected calculated values (note these have been rounded)
    std::vector<double> mu_calc_ref  = {15.5, 13.1, 11.1, 9.76, 10.6, 12.7, 15.3, 13.9, 9.25, 8.27};

    Default_Data_Store d;
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));
    
    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.727,0.157,0.116}));

    std::cout << "Density:" << std::endl
              << " T(c) experimental calc_ref calced" << std::endl;
    
    for( size_t i = 0; i < tcs.size(); ++i)
    {
        double t_c = tcs[i];        
        EXPECT_NEAR(mu_calc_ref[i], tp.mu(utils::c2k(t_c)), 5e-2);
        // print data
        std::cout << t_c << " " << mu_exp[i] 
                         << " " << mu_calc_ref[i] 
                         << " " << tp.mu(utils::c2k(t_c)) 
                         << " " << mu_calc_ref[i] - tp.mu(utils::c2k(t_c)) << std::endl;
    }
}