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
    {
    std::vector<std::string> ref = {""};
    EXPECT_EQ(ref, utils::split("-","-"));
    }

    {
    std::vector<std::string> ref = {"MgCl"};
    EXPECT_EQ(ref, utils::split("-","MgCl-"));
    }

    {
    std::vector<std::string> ref = {"MgCl","KCl"};
    EXPECT_EQ(ref, utils::split("-","MgCl-KCl"));
    }

    {
    std::vector<std::string> ref = {"MgCl","KCl","LiF"};
    EXPECT_EQ(ref, utils::split("-","MgCl-KCl-LiF"));
    }

    {
    std::vector<std::string> ref = {"MgCl","KCl","LiF","BeF"};
    EXPECT_EQ(ref, utils::split("-","MgCl-KCl-LiF-BeF"));
    EXPECT_EQ(ref, utils::split("-","MgCl-KCl-LiF-BeF-"));
    ref.insert(ref.begin(),"");
    EXPECT_EQ(ref, utils::split("-","-MgCl-KCl-LiF-BeF-"));
    ref.insert(ref.begin()+3,"");
    EXPECT_EQ(ref, utils::split("-","-MgCl-KCl--LiF-BeF-"));
    }
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
    std::vector<double> a = {1, 1};
    std::vector<double> b = {1, 1};
    EXPECT_EQ(0, utils::euclidean_distance(a,b));
    EXPECT_EQ(0, utils::euclidean_distance(b,a));
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
TEST(salineutils,ltrim)
{
    {
    std::string test = "";
    std::string ref = "";
    utils::ltrim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "";
    std::string ref = "";
    utils::ltrim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = " LiF";
    std::string ref = "LiF";
    utils::ltrim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "    LiF     ";
    std::string ref = "LiF     ";
    utils::ltrim(test);
    EXPECT_EQ(ref,test);
    }
}
TEST(salineutils,rtrim)
{
    {
    std::string test = "";
    std::string ref = "";
    utils::rtrim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "";
    std::string ref = "";
    utils::rtrim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "LiF ";
    std::string ref = "LiF";
    utils::rtrim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "    LiF    ";
    std::string ref = "    LiF";
    utils::rtrim(test);
    EXPECT_EQ(ref,test);
    }
}
TEST(salineutils,trim)
{
    {
    std::string test = "";
    std::string ref = "";
    utils::trim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "";
    std::string ref = "";
    utils::trim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "LiF ";
    std::string ref = "LiF";
    utils::trim(test);
    EXPECT_EQ(ref,test);
    }

    {
    std::string test = "    LiF    ";
    std::string ref = "LiF";
    utils::trim(test);
    EXPECT_EQ(ref,test);
    }
}
TEST(salineutils,getSortPermutation)
{
    //empty
    {
    std::vector<double> challenge;
    std::vector<size_t> ref;
    auto test = utils::getSortPermutation(challenge);
    EXPECT_TRUE(test.empty());
    }
    //doubles
    {
    std::vector<double> challenge = {0.0,1.0,2.0,3.0,4.0};
    std::vector<size_t> ref = {0,1,2,3,4};
    auto test = utils::getSortPermutation(challenge);
    EXPECT_EQ(ref,test);
    }
    {
    std::vector<double> challenge = {0.0,3.0,2.0,4.0,1.0};
    std::vector<size_t> ref = {0,4,2,1,3};
    auto test = utils::getSortPermutation(challenge);
    EXPECT_EQ(ref,test);
    }
    //integers
    {
    std::vector<int> challenge = {0,1,2,3,4};
    std::vector<size_t> ref = {0,1,2,3,4};
    auto test = utils::getSortPermutation(challenge);
    EXPECT_EQ(ref,test);
    }
    {
    std::vector<int> challenge = {1,2,0,4,3};
    std::vector<size_t> ref = {2,0,1,4,3};
    auto test = utils::getSortPermutation(challenge);
    EXPECT_EQ(ref,test);
    }
    //characters
    {
    std::vector<std::string> challenge = {"a","b","c","d","e"};
    std::vector<size_t> ref = {0,1,2,3,4};
    auto test = utils::getSortPermutation(challenge);
    EXPECT_EQ(ref,test);
    }
    {
    std::vector<std::string> challenge = {"c","b","d","e","a"};
    std::vector<size_t> ref = {4,1,0,2,3};
    auto test = utils::getSortPermutation(challenge);
    EXPECT_EQ(ref,test);
    }
}

TEST(salineutils,applySortPermutation)
{
    //empty
    {
    std::vector<double> challenge;
    std::vector<size_t> ref;
    auto sp = utils::getSortPermutation(challenge);
    auto test = utils::applySortPermuation(challenge,sp);
    EXPECT_TRUE(test.empty());
    }
    //doubles
    {
    std::vector<double> challenge = {0.0,1.0,2.0,3.0,4.0};
    std::vector<double> ref = {0.0,1.0,2.0,3.0,4.0};
    auto sp = utils::getSortPermutation(challenge);
    auto test = utils::applySortPermuation(challenge,sp);
    EXPECT_EQ(ref,test);
    }
    {
    std::vector<double> challenge = {0.0,3.0,2.0,4.0,1.0};
    std::vector<double> ref = {0.0,1.0,2.0,3.0,4.0};
    auto sp = utils::getSortPermutation(challenge);
    auto test = utils::applySortPermuation(challenge,sp);
    EXPECT_EQ(ref,test);
    }
    //integers
    {
    std::vector<int> challenge = {1,2,0,4,3};
    std::vector<int> ref = {0,1,2,3,4};
    auto sp = utils::getSortPermutation(challenge);
    auto test = utils::applySortPermuation(challenge,sp);
    EXPECT_EQ(ref,test);
    }
    //characters
    {
    std::vector<std::string> challenge = {"c","b","d","e","a"};
    std::vector<std::string> ref = {"a","b","c","d","e"};
    auto sp = utils::getSortPermutation(challenge);
    auto test = utils::applySortPermuation(challenge,sp);
    EXPECT_EQ(ref,test);
    }
}

TEST(salineutils,applySortPermuation_inPlace)
{
    //empty
    {
    std::vector<double> test;
    std::vector<size_t> ref;
    auto sp = utils::getSortPermutation(test);
    utils::applySortPermuation_inPlace(test,sp);
    EXPECT_TRUE(test.empty());
    }
    //doubles
    {
    std::vector<double> test = {0.0,1.0,2.0,3.0,4.0};
    std::vector<double> ref = {0.0,1.0,2.0,3.0,4.0};
    auto sp = utils::getSortPermutation(test);
    utils::applySortPermuation_inPlace(test,sp);
    EXPECT_EQ(ref,test);
    }
    {
    std::vector<double> test = {0.0,3.0,2.0,4.0,1.0};
    std::vector<double> ref = {0.0,1.0,2.0,3.0,4.0};
    auto sp = utils::getSortPermutation(test);
    utils::applySortPermuation_inPlace(test,sp);
    EXPECT_EQ(ref,test);
    }
    //integers
    {
    std::vector<int> test = {1,2,0,4,3};
    std::vector<int> ref = {0,1,2,3,4};
    auto sp = utils::getSortPermutation(test);
    utils::applySortPermuation_inPlace(test,sp);
    EXPECT_EQ(ref,test);
    }
    //characters
    {
    std::vector<std::string> test = {"c","b","d","e","a"};
    std::vector<std::string> ref = {"a","b","c","d","e"};
    auto sp = utils::getSortPermutation(test);
    utils::applySortPermuation_inPlace(test,sp);
    EXPECT_EQ(ref,test);
    }
}
