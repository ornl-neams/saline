#include "gtest/gtest.h"

#include "thermophysical_properties.hh"
#include "default_data_store.hh"
#include "r_kister_data_store.hh"
#include "default_data.hh"

using namespace saline;

TEST(rk_data_store,load)
{
    // Obtain the base data
    Default_Data_Store ds;
    ds.load();
    Thermophysical_Properties tp_dflt;
    ASSERT_TRUE(tp_dflt.initialize(&ds));
    ASSERT_TRUE(tp_dflt.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42}));

    // Set up the interpolation object
    R_Kister_Data_Store rk_DS; rk_DS.load();
    Thermophysical_Properties tp_rk;
    ASSERT_TRUE(tp_rk.initialize(&rk_DS));

    tp_rk.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42});

    // Using values from reference https://doi.org/10.1021/je00029a041
    std::vector<double> tks           = {1080,1130,1180,1230};

    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(tp_rk.rho(t_k),tp_dflt.rho(t_k),5e-2);
    }
    EXPECT_FLOAT_EQ(tp_rk.rho(1100)*1000.0,tp_rk.rho_kgm3(1100));
}

// Test to make sure we get the same answer regardless of input order
TEST(rk_data_store,input_order)
{
    // Set up the interpolation object
    R_Kister_Data_Store rk_DS; rk_DS.load();
    Thermophysical_Properties tp_rk;
    ASSERT_TRUE(tp_rk.initialize(&rk_DS));

    std::vector<double> tks           = {1080,1130,1180,1230};
    tp_rk.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42});
    std::vector<double> ref(tks.size());
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        ref[i] = tp_rk.rho(t_k);
    }

    tp_rk.setComposition({"NaF","LiF","KF"},{0.115,0.465,0.42});
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(tp_rk.rho(t_k),ref[i],1e-6);
    }

    tp_rk.setComposition({"KF","NaF","LiF"},{0.42,0.115,0.465});
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(tp_rk.rho(t_k),ref[i],1e-6);
    }
}
