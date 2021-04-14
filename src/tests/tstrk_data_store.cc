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
    Default_Data_Store dflt_DS;
    std::istringstream in(default_data_rk);
    dflt_DS.load(in);
    R_Kister_Data_Store rk_DS;
    rk_DS.load(&dflt_DS);
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
}
