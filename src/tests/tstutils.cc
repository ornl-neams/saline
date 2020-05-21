#include "gtest/gtest.h"

#include <vector>
#include "utils.hh"

using namespace saline;

TEST(salineutils, celius_2_kelvin)
{
    EXPECT_EQ(273.15, utils::c2k(0));
    EXPECT_EQ(273.15*2, utils::c2k(273.15));
}


TEST(salineutils, split)
{
    std::vector<std::string> ref = {"MgCl","KCl"};
    EXPECT_EQ(ref, utils::split("-","MgCl-KCl"));    
}