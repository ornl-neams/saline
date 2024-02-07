#include "gtest/gtest.h"

#include "thermophysical_properties.hh"
#include "default_data_store.hh"
#include "r_kister_data_store.hh"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace saline;

static const char* tst_data = R"ORNL_format(
//Pure Salts
//System      , id , Mol Mass , Mol Frac             , Melt(K) , Var(%) , Reference   , Boil(K) , Var(%) , Reference      , rho_a     , rho_b     , range(K)      , Var(%) , Reference     , mu1_a     , mu2_b     , mu2_a      , mu2_b      , mu2_c     , range(K)      , Var(%) , Reference    , k_a        , k_b        , range(K)     , Var(%) , Reference   , cp_a      , cp_b       , cp_c       , cp_d      , Var(%) , Reference
LiF           , 1  , 25.9390  , 1.0                  , 1121.20 , 1.00   , Douglas1954 , 1943.00 , 0.00   , Ruff1922       , 2.371E+00 , 5.000E-04 , 1123.6-1367.5 , 1.00   , Hill1967      , 1.149E-01 , 2.699E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1133-1772     , 1.00   , Abe1981      , 1.882E+00  , -3.990E-04 , 1118.5-1900  , 20.00  , Gheribi2014 , 6.489E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1.50   , Douglas1954
NaF           , 2  , 41.9882  , 1.0                  , 1268.00 , 0.50   , Cantor1961  , 1978.00 , 0.00   , Ruff1922       , 2.755E+00 , 6.360E-04 , 1273-1373     , 0.20   , Paucirova1970 , 1.196E-01 , 2.649E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1273-1373     , 1.00   , Brockner1979 , 1.259E+00  , -2.800E-04 , 1268.15-1800 , 20.00  , Gheribi2014 , 6.862E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.10   , O'Brien1957
KF            , 3  , 58.0967  , 1.0                  , 1131.20 , 0.50   , Johnson1958 , 1771.00 , 0.00   , Ruff1922       , 2.646E+00 , 6.515E-04 , 1154.2-1310.2 , 1.00   , Yaffe1956     , 1.068E-01 , 2.378E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1141.2-1327.6 , 2.00   , Janz1988     , 8.552E-01  , -2.540E-04 , 1129.15-1800 , 20.00  , Gheribi2014 , 7.060E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 18.00  , Beilmann2013
BeF2          , 4  , 47.0100  , 1.0                  , 821.15  , 1.00   , Thoma1960   , 1442.00 , 0.00   , Cantor1965     , 1.972E+00 , 1.450E-05 , 1073-1123     , 0.50   , Cantor1969    , 0.000E+00 , 0.000E+00 , -8.119E+00 , 1.149E+04  , 6.390E+05 , 846.9-1252.2  , 3.00   , Moynihan1968 , 8.007E-01  , -2.120E-06 , 1070.15-0.0  , 20.00  , Gheribi2014 , 1.027E+02 , -1.539E-03 , -1.565E+07 , 3.000E-09 , 0.00   , Glushko1994
ThF4          , 5  , 308.0300 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 7.080E+00 , 0.000E+00 , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----
UF3           , 6  , 295.0200 , 1.0                  , 1768.15 , 20.00  , D'Eye1957   , 2550.00 , 0.00   , Glushko1994    , 0.000E+00 , 0.000E+00 , 0.0-0.0       , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 1.300E+02 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 5.00   , Beilmann2013a
ZrF4          , 7  , 167.2100 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 5.368E+00 , 5.3677    , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----
LiF-NaF-KF    , 8  , 41.2909  , 0.465-0.115-0.42     , 735.00  , 2.00   , Rogers1982  , 1843.15 , 0.0    , Williams 2006b , 2.579E+00 , 6.240E-04 , 933-1163      , 1.00   , Cibulkova2006 , 0.000E+00 , 0.000E+00 , 2.130E-01  , -1.200E+03 , 1.350E+06 , 770-970       , 2.00   , Toerklep1980 , -3.500E-01 , 1.300E-03  , 773-973      , 3.50   , An2015      , 4.032E+01 , 4.388E-02  , 0.000E+00  , 0.000E+00 , 2.00   , Rogers1982
LiF-BeF2-ThF4 , 9  , 47.9244  , 0.7011-0.2388-0.0601 , 828.30  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 3.295E+00 , 6.707E-04 , 828.3-980.6   , 1.00   , Cantor1973    , 6.602E-02 , 3.642E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 799-926       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----
LiF-BeF2-ThF4 , 10 , 63.5179  , 0.7006-0.1796-0.1198 , 806.40  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.044E+00 , 8.064E-04 , 806.4-1014.4  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----
LiF-BeF2-ThF4 , 11 , 71.4958  , 0.6998-0.1499-0.1503 , 816.60  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.441E+00 , 9.526E-04 , 816.6-1022.7  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----
LiF-BeF2-ThF4 , 12 , 61.9697  , 0.727-0.157-0.116    , 826.20  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 0.0       , 0.0       , 0.0-0.0       , 0.0    , ----          , 1.094E-01 , 3.402E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 826-946       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----
)ORNL_format";

static const char* tst_data_rk = R"ORNL_format(//
//C 1 , C 2     , A1            , B1            , A2            , B2            , A3           , B3           , T min        , T max        , Reference
KF    , LiF     , -5.383100E-03 , -4.142700E-05 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.006150E+03 , 1.314150E+03 , 'Taniuchi, K.; Kanai, T. Density of Binary Molten Salts of Lithium Fluoride-Potassium Fluoride and Lithium Fluoride-Calcium Fluoride Systems. Denki Kagaku oyobi Kogyo Butsuri Kagaku 1977, 45 (6), 401-404. https://doi.org/10.5796/kogyobutsurikagaku.45.401'
KF    , NaF     , -3.747500E-01 , +2.354000E-04 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.050000E+03 , 1.350000E+03 , 'Porter, B., and Meaker, R. E., United States Department of the Interior, Report of Investigations 6838, 1966.'
LiF   , NaF     , -6.998400E-02 , +1.073900E-05 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.130000E+03 , 1.320000E+03 , 'Matiasovsky, K., Private communication, 1968.'
NaF   , BeF2    , +7.850000E-01 , -7.850000E-04 , -1.600000E+00 , +1.540000E-04 , 0.000000E+00 , 0.000000E+00 , 8.730000E+02 , 1.073000E+03 , 'Blanke, B. C., Bousquet, L. V., Jones, L. V., Murphy, E. L., & Vallee, R. E. (1958). Density of fused mixtures of sodium fluoride, beryllium fluoride, and uranium fluoride. Miamisburg, Ohio: Mound Laboratory, Monsanto Chemical Company, U.S. Atomic Energy Commission.'
LiF   , BeF2    , +8.930000E-01 , -8.960000E-04 , +3.780000E-01 , -1.160000E-04 , 0.000000E+00 , 0.000000E+00 , 8.000000E+02 , 1.130000E+03 , 'Cantor, S., Ward, W. T., & Moynihan, C. T. (1969). Viscosity and Density in Molten BeF 2 – LiF Solutions. J Chem. Phys., 50(7), 2874-2879.   Cantor, S. (1970). Density of Molten Fuorides of Reactor Interest. In M. W. Rosenthal, R. B. Brigss, & P. R. Kasten, U.S.A.E.C Semiannual Progress Report ORNL-4449 (pp. 145-146). Oak Ridge, TN: Oak Ridge National Laboratory.'
ThF4  , KF      , -4.757300E-01 , -4.099400E-06 , +1.626800E+00 , -4.418000E-04 , 0.000000E+00 , 0.000000E+00 , 1.046000E+03 , 1.325000E+03 , 'Desyatnik, V. N.; Klimenkov, A. A.; Kurbatov, N. N.; Nechaev, A. I.; Raspopin, S. P.; Chervinskii, Y. F. Density and Kinematic Viscosity of NaF-ThF4 and KF-ThF4 Melts. Sov. At. Energy 1981, 51 (6), 807-810. https://doi.org/10.1007/BF01121687.'
ThF4  , LiF     , +9.640500E-01 , -8.240700E-04 , +0.000000E+00 , +0.000000E+00 , 0.000000E+00 , 0.000000E+00 , 1.016650E+03 , 1.508150E+03 , 'Hill D. G., Cantor S., Ward W. T. Molar volumes in the LiF-ThF4 system. J. Inorg. Nucl. Chem. 1967, 29, 241-243.'
ThF4  , NaF     , +8.059200E-01 , -9.775800E-04 , +2.479900E+00 , -1.254100E-03 , 0.000000E+00 , 0.000000E+00 , 1.003000E+03 , 1.337000E+03 , 'Desyatnik, V. N.; Klimenkov, A. A.; Kurbatov, N. N.; Nechaev, A. I.; Raspopin, S. P.; Chervinskii, Y. F. Density and Kinematic Viscosity of NaF-ThF4 and KF-ThF4 Melts. Sov. At. Energy 1981, 51 (6), 807810. https://doi.org/10.1007/BF01121687.'
LiF   , ZrF4    , -1.282600E+00 , -4.689000E-04 , +7.520000E-01 , +2.322500E-04 , 0.000000E+00 , 0.000000E+00 , 9.430000E+02 , 1.280000E+03 , 'Katyshev, S. F.; Artemov, V. V; Desyatnik, V. N. Density and Surface Tension of Melts of Zirconium and Hafnium Fluorides with Lithium Fluoride. Sov. At. Energy 1987, 63 (6), 929-930. https://doi.org/10.1007/BF01126108.'
ZrF4  , NaF     , -1.186600E+00 , +2.107500E-04 , +2.082200E-01 , -4.726400E-04 , 0.000000E+00 , 0.000000E+00 , 9.530000E+02 , 1.280000E+03 , 'Artemov, V. V.; Katyshev, S. F.; Desyatnik, V. N. Density and Surface Tension of Sodium Halide Melts with Zirconium and Hafnium Tetrafluorides. Zhurnal Fiz. Khimii 1990, 64 (4), 1113-1115.'
)ORNL_format";//string representation of csv

TEST(rk_data_store,load)
{
    // Obtain the base data
    Default_Data_Store ds;
    std::istringstream in(tst_data);
    ds.load(in);
    Thermophysical_Properties tp_dflt;
    ASSERT_TRUE(tp_dflt.initialize(&ds));
    ASSERT_TRUE(tp_dflt.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42}));

    // Set up the interpolation object
    std::vector<std::string> right_names = {"LiF","NaF","KF"};
    std::vector<std::string> wrong_names = {"LiF2","NaF","KF"};
    std::istringstream ind(tst_data);
    std::istringstream in_rk(tst_data_rk);
    R_Kister_Data_Store rk_DS; rk_DS.load(in_rk,ind);
    Thermophysical_Properties tp_rk;
    ASSERT_TRUE(tp_rk.initialize(&rk_DS));
    ASSERT_TRUE(rk_DS.valid(right_names));
    ASSERT_TRUE(!rk_DS.valid(wrong_names));

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
    std::istringstream in(tst_data);
    std::istringstream inrk(tst_data_rk);
    R_Kister_Data_Store rk_DS; rk_DS.load(inrk,in);
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

    // Set an incomplete data sets to test validity functions
    tp_rk.setComposition({"LiF","UF3","ZrF4"},{0.80,0.07,0.13});
    ASSERT_FALSE(tp_rk.valid_rho());
    ASSERT_FALSE(tp_rk.valid_k());
    ASSERT_FALSE(tp_rk.valid_cp());
    ASSERT_FALSE(tp_rk.valid_mu());
}

TEST(rk_data_store,full_load)
{
    // Set up the interpolation object
    R_Kister_Data_Store rk_DS; rk_DS.load("/home/oxh/dev/mstdb-tp/Molten_Salt_Thermophysical_Properties_RK.csv","/home/oxh/dev/mstdb-tp/Molten_Salt_Thermophysical_Properties.csv");
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

    // Set an incomplete data sets to test validity functions
    tp_rk.setComposition({"LiF","UF3","ZrF4"},{0.80,0.07,0.13});
    ASSERT_FALSE(tp_rk.valid_rho());
    ASSERT_FALSE(tp_rk.valid_k());
    ASSERT_FALSE(tp_rk.valid_cp());
    ASSERT_FALSE(tp_rk.valid_mu());
}
