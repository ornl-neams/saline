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
    {
    std::vector<double> a = {-10};
    std::vector<double> b = {-10};
    EXPECT_EQ(0, utils::euclidean_distance(a,b));
    EXPECT_EQ(0, utils::euclidean_distance(b,a));
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

TEST(salineutils, euclidean_distance_rank_error)
{

    std::vector<double> a = {0, 0, 0};
    std::vector<double> b = {0, 0};
    EXPECT_ANY_THROW(utils::euclidean_distance(a,b));
}

TEST(salineutils, euclidean_distance_empty_error)
{

    std::vector<double> a;
    std::vector<double> b;
    EXPECT_ANY_THROW(utils::euclidean_distance(a,b));
}

TEST(salineutils, nearest_neighbor)
{
    std::vector<double> a = {0.6998,0.1499,0.1503};
    std::vector<std::vector<double>> neighbors = {{0.7011,0.2388,0.0601},
                                                  {0.7006,0.1796,0.1198},
                                                  {0.6998,0.1499,0.1503},
                                                  {0.727,0.157,0.116}};
    auto nearest = utils::nearest_neighbor(a, neighbors);
    
    // for( auto n : nearest) std::cout <<n.first<<", "<<n.second<< std::endl;

    std::vector<std::pair<double, size_t>> ref = {{0, 2}, 
                                                  {0.0425791, 1},
                                                  {0.0443479, 3},
                                                  {0.126653, 0}};
    for(size_t i = 0; i < ref.size(); ++i)
    {
        SCOPED_TRACE(i);
        
        EXPECT_NEAR(ref[i].first, nearest[i].first, 1e-5);
        EXPECT_EQ(ref[i].second, nearest[i].second);
    }
    
}