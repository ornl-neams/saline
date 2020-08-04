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

TEST(salineutils, euclidean_distance_1d)
{
    {
    std::vector<double> a = {0};
    std::vector<double> b = {1};
    EXPECT_EQ(1, utils::euclidean_distance(a,b));
    EXPECT_EQ(1, utils::euclidean_distance(b,a));
    }
    {
    std::vector<double> a = {-1};
    std::vector<double> b = {1};
    EXPECT_EQ(2, utils::euclidean_distance(a,b));
    EXPECT_EQ(2, utils::euclidean_distance(b,a));
    }
}

TEST(salineutils, euclidean_distance_2d)
{
    {
    std::vector<double> a = {0, 0};
    std::vector<double> b = {1, 0};
    EXPECT_EQ(1, utils::euclidean_distance(a,b));
    EXPECT_EQ(1, utils::euclidean_distance(b,a));
    }
    {
    std::vector<double> a = {-1, -1};
    std::vector<double> b = {1, -1};
    EXPECT_EQ(2, utils::euclidean_distance(a,b));
    EXPECT_EQ(2, utils::euclidean_distance(b,a));
    }
}

TEST(salineutils, euclidean_distance_3d)
{
    {
    std::vector<double> a = {0, 0, 0};
    std::vector<double> b = {0, 0, 3};
    EXPECT_EQ(3, utils::euclidean_distance(a,b));
    EXPECT_EQ(3, utils::euclidean_distance(b,a));
    }
    {
    std::vector<double> a = {-2, -2, -2};
    std::vector<double> b = {2, -2, -2};
    EXPECT_EQ(4, utils::euclidean_distance(a,b));
    EXPECT_EQ(4, utils::euclidean_distance(b,a));
    }
}