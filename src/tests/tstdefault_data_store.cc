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

    Default_Data_Store d; d.load();
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
        EXPECT_NEAR(rho_calc_ref[i], tp.rho(utils::c2k(t_c)), 1e-3);
        // print data
        std::cout << t_c << " " << rho_exp[i]
                         << " " << rho_calc_ref[i]
                         << " " << tp.rho(utils::c2k(t_c))
                         << " " << rho_calc_ref[i] - tp.rho(utils::c2k(t_c)) << std::endl;
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

    Default_Data_Store d; d.load();
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
        EXPECT_NEAR(rho_calc_ref[i], tp.rho(utils::c2k(t_c)), 1e-3);
        // print data
        std::cout << t_c << " " << rho_exp[i]
                         << " " << rho_calc_ref[i]
                         << " " << tp.rho(utils::c2k(t_c))
                         << " " << rho_calc_ref[i] - tp.rho(utils::c2k(t_c)) << std::endl;
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

    Default_Data_Store d; d.load();
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

TEST(default_data, mu_LiFBeF2ThF4_7011_2388_601)
{
    // measured t(c) for LiF-BeF2-ThF4 @ 70.11, 23.88, 6.01 mole %) Cantor '73 (https://www.osti.gov/servlets/purl/4419855)
    std::vector<double> tcs           = {653, 547, 598, 633, 526, 567, 579, 603, 557};
    // Experimental values
    std::vector<double> mu_exp       = {7.30, 14.15, 9.88, 8.97, 16.53, 12.56, 11.06, 10.19, 12.69};
    // Expected calculated values (note these have been rounded)
    std::vector<double> mu_calc_ref  = {7.47, 13.8, 10.1, 8.30, 15.8, 12.1, 11.3, 9.79, 12.9};

    Default_Data_Store d; d.load();
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.7011, 0.2388, 0.0601}));

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

//Effectively this test the assignment of the nearest composition
TEST(default_data, mu_LiFBeF2ThF4_7001_2388_611)
{
    // measured t(c) for LiF-BeF2-ThF4 @ 70.11, 23.88, 6.01 mole %) Cantor '73 (https://www.osti.gov/servlets/purl/4419855)
    std::vector<double> tcs           = {653, 547, 598, 633, 526, 567, 579, 603, 557};
    // Experimental values
    std::vector<double> mu_exp       = {7.30, 14.15, 9.88, 8.97, 16.53, 12.56, 11.06, 10.19, 12.69};
    // Expected calculated values (note these have been rounded)
    std::vector<double> mu_calc_ref  = {7.47, 13.8, 10.1, 8.30, 15.8, 12.1, 11.3, 9.79, 12.9};

    Default_Data_Store d; d.load();
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.7001, 0.2388, 0.0611}));

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
TEST(default_data, mu_LiFBeF2ThF4_7140_2259_601)
{
    // measured t(c) for LiF-BeF2-ThF4 @ 70.11, 23.88, 6.01 mole %) Cantor '73 (https://www.osti.gov/servlets/purl/4419855)
    std::vector<double> tcs           = {653, 547, 598, 633, 526, 567, 579, 603, 557};
    // Experimental values
    std::vector<double> mu_exp       = {7.30, 14.15, 9.88, 8.97, 16.53, 12.56, 11.06, 10.19, 12.69};
    // Expected calculated values (note these have been rounded)
    std::vector<double> mu_calc_ref  = {7.47, 13.8, 10.1, 8.30, 15.8, 12.1, 11.3, 9.79, 12.9};

    Default_Data_Store d; d.load();
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.7140, 0.2259, 0.0601}));

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

TEST(default_data, rho_cp_mu_k_LiFNaFKF_465_115_042)
{
    Default_Data_Store d; d.load();
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    ASSERT_TRUE(tp.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.042}));

    EXPECT_NEAR(2.01770, tp.rho(900), 1e-5);
    EXPECT_NEAR(tp.rho_h(tp.h_t(900)), tp.rho(900), 1e-4);

    // Using values from reference https://doi.org/10.1021/je00029a041
    std::vector<double> tks           = {750, 800, 850, 860};
    // Experimental values
    // NOTE reference values are {17.50, 18.03, 18.55, 18.60} cal mol^-1 K^-1, but we use J mol^-1 K^-1
    std::vector<double> cp_exp       = {73.22, 75.43752, 75.43752, 77.8224};
    // Expected calculated values (note these have been rounded)
    std::vector<double> cp_calc_ref  = {73.2255, 75.4194, 77.6133, 78.0521};
    
    std::cout << "Heat Capacity:" << std::endl
              << " T(K) experimental calc_ref calced" << std::endl;
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(cp_calc_ref[i], tp.cp(t_k), 5e-4);
        // print data
        std::cout << t_k << " " << cp_exp[i]
                         << " " << cp_calc_ref[i]
                         << " " << tp.cp(t_k)
                         << " " << cp_calc_ref[i] - tp.mu(t_k) << std::endl;
    }
    EXPECT_NEAR(tp.cp_h(tp.h_t(1200)), tp.cp(1200), 4e-2);
    tks.clear();

    // Using values from reference https://doi.org/10.1021/je60084a007
    tks                              = {822.45, 872.65, 973.45, 922.95, 770.15};
    // Experimental values
    std::vector<double> mu_exp       =  {5.615,  4.097,  2.535,  3.156,  8.551}; 
    // Expected calculated value
    std::vector<double> mu_calc_ref  = {5.6296, 4.0862, 2.5436, 3.1494, 8.5459};

    std::cout << "Viscosity :" << std::endl
              << " T(K) experimental calc_ref calced" << std::endl;
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(mu_calc_ref[i], tp.mu(t_k), 5e-4);
        // print data
        std::cout << t_k << " " << mu_exp[i]
                         << " " << mu_calc_ref[i]
                         << " " << tp.mu(t_k)
                         << " " << mu_calc_ref[i] - tp.mu(t_k) << std::endl;
    }
    tks.clear();
    
    EXPECT_NEAR(tp.mu_h(tp.h_t(900)), tp.mu(900), 6e-4);

    // Using values from reference https://doi.org/10.1016/j.ijheatmasstransfer.2015.07.042
    tks                              = {773.0,  823.0,  873.0,  923.0,  973.0};
    // Experimental values
    std::vector<double> k_exp       = {0.6520, 0.7730, 0.7720, 0.8320, 0.9270}; 
    // Expected calculated value
    std::vector<double> k_calc_ref  = {0.6549, 0.7199, 0.7849, 0.8499, 0.9149};

    std::cout << "Thermal Conductivity :" << std::endl
              << " T(K) experimental calc_ref calced" << std::endl;
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(k_calc_ref[i], tp.k(t_k), 5e-4);
        // print data
        std::cout << t_k << " " << k_exp[i]
                         << " " << k_calc_ref[i]
                         << " " << tp.k(t_k)
                         << " " << k_calc_ref[i] - tp.mu(t_k) << std::endl;
    }
    EXPECT_NEAR(tp.k_h(tp.h_t(970)), tp.k(970), 1e-4);

    EXPECT_NEAR(748.77235618, tp.t_h(1000), 1e-6);
    EXPECT_NEAR(tp.t_h(tp.h_t(970)),970,5e-2);
}
