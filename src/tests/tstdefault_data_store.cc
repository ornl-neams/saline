#include "gtest/gtest.h"

#include "../default_data_store.hh"
#include "../thermophysical_properties.hh"
#include "../utils.hh"

#include <iostream>

using namespace saline;

TEST(default_data, test)
{
    Default_Data_Store d;
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    ASSERT_TRUE(tp.setComposition({"BeCl2"},{1.0}));
    std::cout <<"BeCl2 ... "  <<std::endl;
    std::cout << "rho : "<< tp.rho(973.15) << " to " << tp.rho(2000) << std::endl;
    std::cout << "enthalpy : " << tp.h_t(698) << " to " << tp.h_t(750) << std::endl;
    std::cout << "temperature : " << tp.t_h(tp.h_t(698)) << " to " << tp.t_h(tp.h_t(750)) << std::endl;

    std::cout << "enthalpy : " << tp.h_t(658) << " to " << tp.h_t(850) << std::endl;
    std::cout << "temperature : " << tp.t_h(tp.h_t(658)) << " to " << tp.t_h(tp.h_t(850)) << std::endl;

    ASSERT_TRUE(tp.setComposition({"KCl","MgCl2"},{0.68,0.32}));
    std::cout <<"MgCl2-KCl..." << std::endl;

    // expecting ~0.47 and ~.43 + or - 3 cP
    std::cout << tp.mu(utils::c2k(450)) <<  " " << tp.mu(utils::c2k(800)) << std::endl;
    // expecting ~0.47 and ~.43 + or - 3 cP
    EXPECT_EQ(0.47, tp.mu(utils::c2k(450)));
    EXPECT_EQ(0.0126694251, tp.cp(utils::c2k(500)));

    // expecting ~1680 and ~1490 + or - 25 kg/m^3
    std::cout << tp.rho(utils::c2k(450)) <<  " " << tp.rho(utils::c2k(800)) << std::endl;


    ASSERT_TRUE(tp.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42}));
    std::cout <<"LiF-NaF-KF..." << std::endl;

    // expecting ~2.9  cP
    std::cout << tp.mu(utils::c2k(700)) << std::endl;
    // expecting ~0.9 W/m K
    std::cout << tp.k(utils::c2k(700)) << std::endl;

    ASSERT_TRUE(tp.setComposition({"LiF","BeF2","ThF4"},{0.6998,0.1499,0.1503}));

    for( double t_c = 550; t_c <= 800; t_c+=50)
    {
          std::cout << t_c << " " << tp.mu(utils::c2k(t_c)) << std::endl;
    }

}