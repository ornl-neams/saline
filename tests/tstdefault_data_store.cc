#include "gtest/gtest.h"

#include "default_data_store.hh"
#include "thermophysical_properties.hh"
#include "utils.hh"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace saline;

static const char *tst_data = R"ORNL_format(
//Pure Salts
//System       , Mol Mass , Mol Frac             , Melt(K) , Var(%) , Reference   , Boil(K) , Var(%) , Reference      , rho_a     , rho_b     , range(K)      , Var(%) , Reference     , mu1_a     , mu2_b     , mu2_a      , mu2_b      , mu2_c     , range(K)      , Var(%) , Reference    , k_a        , k_b        , range(K)     , Var(%) , Reference   , cp_a      , cp_b       , cp_c       , cp_d      , Var(%) , Reference, st_a, st_b, range(K), Var(%), Reference
LiF            , 25.9390  , 1.0                  , 1121.20 , 1.00   , Douglas1954 , 1943.00 , 0.00   , Ruff1922       , 2.371E+00 , 5.000E-04 , 1123.6-1367.5 , 1.00   , Hill1967      , 1.149E-01 , 2.699E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1133-1772     , 1.00   , Abe1981      , 1.882E+00  , -3.990E-04 , 1118.5-1900  , 20.00  , Gheribi2014 , 6.489E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1.50   , Douglas1954, ----, ----, ----, ----, ----
NaF            , 41.9882  , 1.0                  , 1268.00 , 0.50   , Cantor1961  , 1978.00 , 0.00   , Ruff1922       , 2.755E+00 , 6.360E-04 , 1273-1373     , 0.20   , Paucirova1970 , 1.196E-01 , 2.649E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1273-1373     , 1.00   , Brockner1979 , 1.259E+00  , -2.800E-04 , 1268.15-1800 , 20.00  , Gheribi2014 , 6.862E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.10   , O'Brien1957, ----, ----, ----, ----, ----
KF             , 58.0967  , 1.0                  , 1131.20 , 0.50   , Johnson1958 , 1771.00 , 0.00   , Ruff1922       , 2.646E+00 , 6.515E-04 , 1154.2-1310.2 , 1.00   , Yaffe1956     , 1.068E-01 , 2.378E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1141.2-1327.6 , 2.00   , Janz1988     , 8.552E-01  , -2.540E-04 , 1129.15-1800 , 20.00  , Gheribi2014 , 7.060E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 18.00  , Beilmann2013, ----, ----, ----, ----, ----
BeF2           , 47.0100  , 1.0                  , 821.15  , 1.00   , Thoma1960   , 1442.00 , 0.00   , Cantor1965     , 1.972E+00 , 1.450E-05 , 1073-1123     , 0.50   , Cantor1969    , 0.000E+00 , 0.000E+00 , -8.119E+00 , 1.149E+04  , 6.390E+05 , 846.9-1252.2  , 3.00   , Moynihan1968 , 8.007E-01  , -2.120E-06 , 1070.15-0.0  , 20.00  , Gheribi2014 , 1.027E+02 , -1.539E-03 , -1.565E+07 , 3.000E-09 , 0.00   , Glushko1994, ----, ----, ----, ----, ----
ThF4           , 308.0300 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 7.080E+00 , 0.000E+00 , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----, ----, ----, ----, ----, ----
UF3            , 295.0200 , 1.0                  , 1768.15 , 20.00  , D'Eye1957   , 2550.00 , 0.00   , Glushko1994    , 0.000E+00 , 0.000E+00 , 0.0-0.0       , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 1.300E+02 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 5.00   , Beilmann2013a, ----, ----, ----, ----, ----
ZrF4           , 167.2100 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 5.368E+00 , 5.3677    , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----, ----, ----, ----, ----, ----
LiF-NaF-KF     , 41.2909  , 0.465-0.115-0.42     , 735.00  , 2.00   , Rogers1982  , 1843.15 , 0.0    , Williams 2006b , 2.579E+00 , 6.240E-04 , 933-1163      , 1.00   , Cibulkova2006 , 0.000E+00 , 0.000E+00 , 2.130E-01  , -1.200E+03 , 1.350E+06 , 770-970       , 2.00   , Toerklep1980 , -3.500E-01 , 1.300E-03  , 773-973      , 3.50   , An2015      , 4.032E+01 , 4.388E-02  , 0.000E+00  , 0.000E+00 , 2.00   , Rogers1982, ----, ----, ----, ----, ----
LiF-BeF2-ThF4  , 47.9244  , 0.7011-0.2388-0.0601 , 828.30  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 3.295E+00 , 6.707E-04 , 828.3-980.6   , 1.00   , Cantor1973    , 6.602E-02 , 3.642E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 799-926       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----, ----, ----, ----, ----, ----
LiF-BeF2-ThF4  , 63.5179  , 0.7006-0.1796-0.1198 , 806.40  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.044E+00 , 8.064E-04 , 806.4-1014.4  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----, ----, ----, ----, ----, ----
LiF-BeF2-ThF4  , 71.4958  , 0.6998-0.1499-0.1503 , 816.60  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.441E+00 , 9.526E-04 , 816.6-1022.7  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----, ----, ----, ----, ----, ----
LiF-BeF2-ThF4  , 61.9697  , 0.727-0.157-0.116    , 826.20  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 0.0       , 0.0       , 0.0-0.0       , 0.0    , ----          , 1.094E-01 , 3.402E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 826-946       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----, ----, ----, ----, ----, ----
)ORNL_format";

// Temperatures in Kelvin
static std::vector<double> tVecK = {500.00, 750.00, 800.00, 850.00,
                                    900.00, 950.00, 1000.00};

//---------------------------------------------------------------------------//
// A helper function to consolidate some repeated calls
void compareView_to_DataStore(Data_Store::View &v, Data_Store::Id compId,
                              Data_Store::Id recId) {
  EXPECT_DOUBLE_EQ(v.melt(), v.d->melt(compId, recId));
  EXPECT_DOUBLE_EQ(v.boil(), v.d->boil(compId, recId));

  // Check consistency from view to data store
  for (size_t i = 0; i < tVecK.size(); ++i) {
    EXPECT_FLOAT_EQ(v.rho(tVecK[i]), v.d->rho(compId, recId, tVecK[i]));
    EXPECT_FLOAT_EQ(v.mu(tVecK[i]), v.d->mu(compId, recId, tVecK[i]));
    EXPECT_FLOAT_EQ(v.k(tVecK[i]), v.d->k(compId, recId, tVecK[i]));
    EXPECT_FLOAT_EQ(v.cp(tVecK[i]), v.d->cp(compId, recId, tVecK[i]));
    EXPECT_FLOAT_EQ(v.h_t(tVecK[i]), v.d->h_t(compId, recId, tVecK[i]));
  }
}

// A helper function to consolidate some repeated calls
void compareTemp_to_Enthalpy(Data_Store::View &v) {
  for (size_t i = 0; i < tVecK.size(); ++i) {
    double ht = v.h_t(tVecK[i]);
    EXPECT_FLOAT_EQ(tVecK[i], v.t_h(ht));
    EXPECT_FLOAT_EQ(v.rho(tVecK[i]), v.rho_h(ht));
    EXPECT_FLOAT_EQ(v.mu(tVecK[i]), v.mu_h(ht));
    EXPECT_FLOAT_EQ(v.k(tVecK[i]), v.k_h(ht));
    EXPECT_FLOAT_EQ(v.cp(tVecK[i]), v.cp_h(ht));
  }
}

#ifdef SALINE_USE_HDF5
TEST(default_data_store, load_from_h5) {
  static constexpr double mGas_const = 8.314462618;
  Default_Data_Store d;
  d.load("tsth5.h5");

  // Verify Data was loaded (critical)
  std::vector<std::string> check_names = {"S1", "S2"};
  size_t dSize = 2;
  ASSERT_EQ(dSize, d.size());
  ASSERT_TRUE(d.valid(0));
  ASSERT_TRUE(d.valid(1));
  ASSERT_TRUE(!d.valid(2));
  ASSERT_TRUE(d.valid(check_names[0]));
  ASSERT_TRUE(!d.valid(check_names[1]));
  ASSERT_TRUE(d.valid(check_names));

  // Check that the data id interface works (critical)
  size_t tstID = d.name_to_id(check_names[0]);
  size_t refID = 0;
  ASSERT_EQ(refID, tstID);
  ASSERT_EQ(refID, d.names_to_id({check_names[0]}));
  EXPECT_EQ(1, d.names_to_id(check_names));

  // Check Data directly from data store, i.e. using ID and recID
  size_t nConstituents = d.constituent_count(refID);
  size_t refSize = 1;
  ASSERT_EQ(refSize, nConstituents);
  std::vector<std::string> tstNames = d.names(refID);
  EXPECT_EQ(tstNames[0], "S1");

  // Testing the first record of a given salt
  size_t tstRecID = 0;
  EXPECT_DOUBLE_EQ(500, d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1.00, d.melt_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.melt_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1000, d.boil(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.0, d.boil_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.boil_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.01, d.rho_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.rho_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.01, d.mu_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.mu_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.2, d.k_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.k_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.015, d.cp_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.cp_ref(refID, tstRecID));
  for (size_t i = 0; i < tVecK.size(); ++i) {
    // Density implements a linear model, A-(B*T)
    EXPECT_FLOAT_EQ(1.0 - (0.002 * tVecK[i]), d.rho(refID, tstRecID, tVecK[i]));
    // this item uses A*exp( B / R*T) where R is the molar gas constant constant
    EXPECT_FLOAT_EQ(0.50 * exp(2.0E+4 / (mGas_const * tVecK[i])),
                    d.mu(refID, tstRecID, tVecK[i]));
    // Thermal conductivity implements a linear model, A+(B*T)
    EXPECT_FLOAT_EQ(1.8 + (-3.9E-04 * tVecK[i]),
                    d.k(refID, tstRecID, tVecK[i]));
    // Heat capacity uses a degree 4 polynomial only the scalar portion is used
    // here
    EXPECT_FLOAT_EQ(6.5E01, d.cp(refID, tstRecID, tVecK[i]));
    // h(t) = A * (T - Tmelt);  Tmelt:=500
    double ht = 6.5E01 * (tVecK[i] - 500);
    EXPECT_FLOAT_EQ(ht, d.h_t(refID, tstRecID, tVecK[i]));
    EXPECT_FLOAT_EQ(tVecK[i], d.t_h(refID, tstRecID, ht));

    // Check Temperature/Enthalpy interfaces are consistent
    EXPECT_FLOAT_EQ(d.rho(refID, tstRecID, tVecK[i]),
                    d.rho_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.mu(refID, tstRecID, tVecK[i]),
                    d.mu_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.k(refID, tstRecID, tVecK[i]), d.k_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.cp(refID, tstRecID, tVecK[i]),
                    d.cp_h(refID, tstRecID, ht));
  }

  // Check Data via views
  Data_Store::View v = d.setView({"S1"}, {1.0});

  // basic view info
  ASSERT_TRUE(!v.nullView());
  ASSERT_TRUE(v.d != nullptr);
  EXPECT_EQ(d.constituent_count(refID), v.constituent_count());
  EXPECT_EQ(refID, v.id);
  EXPECT_EQ(tstRecID, v.rec_id);

  EXPECT_DOUBLE_EQ(v.melt(), d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(v.boil(), d.boil(refID, tstRecID));

  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, tstRecID);

  // Check consistency from temperature to enthalpy
  compareTemp_to_Enthalpy(v);
}
#endif

TEST(default_data_store, load_json) {
  static constexpr double mGas_const = 8.314462618;
  static const char *scrap = R"test_data(
    { "MSTDBTP": {
        "evaluated" : {
            "S1": {
                "1": {
                    "boil": { "reference": "ref1", "abs_uncertainty": 0.0, "uncertainty_notes": "None", "value": 1000.0, "value_notes": "None" },
                    "heat_capacity": { "range": [ 0.0, 0.0 ], "reference": "ref1", "pct_uncertainty": 1.5, "uncertainty_notes": "None", "values": [ 65.0, 0.0, 0.0, 0.0 ] },
                    "thermal_conductivity": { "range": [ 500.0, 1000.0 ], "reference": "ref1", "pct_uncertainty": 20.0, "uncertainty_notes": "None", "values": [ 1.8, -0.00039 ] },
                    "melt": { "reference": "ref1", "abs_uncertainty": 1.0, "uncertainty_notes": "None", "value": 500.0, "value_notes": "None" },
                    "viscosity": { "range": [ 500.0, 1000.0 ], "reference": "ref1", "pct_uncertainty": 1.0, "uncertainty_notes": "None", "values": [ 0.5, 2.0E+04 ] },
                    "density": { "range": [ 500.0, 1000.0 ], "reference": "ref1", "pct_uncertainty": 1.0, "uncertainty_notes": "None", "values": [ 1.0, 0.002 ] },
                    "surface_tension": { "range": [ 500.0, 1000.0 ], "reference": "ref1", "pct_uncertainty": 1.0, "uncertainty_notes": "None", "values": [ 1.0, 0.002 ] }
                }
            },
            "S1-S2": {
                "0.36-0.64": {
                    "boil": { "reference": "ref2", "abs_uncertainty": 0.0, "uncertainty_notes": "None", "value": 1000.0, "value_notes": "None" },
                    "heat_capacity": { "range": [ 0.0, 0.0 ], "reference": "ref2", "pct_uncertainty": 0.1, "uncertainty_notes": "None", "values": [ 68.0, 0.0, 0.0, 0.0 ] },
                    "thermal_conductivity": { "range": [ 600.0, 900.0 ], "reference": "ref2", "pct_uncertainty": 0.2, "uncertainty_notes": "None", "values": [ 1.2, -0.00028 ] },
                    "melt": { "reference": "ref2", "abs_uncertainty": 0.5, "uncertainty_notes": "None", "value": 500.0, "value_notes": "None" },
                    "viscosity": { "range": [ 800.0, 1000.0 ], "reference": "ref2", "pct_uncertainty": 1.0, "uncertainty_notes": "None", "values": [ 1.25, 2.0E+04 ] },
                    "density": { "range": [ 500.0, 750.0 ], "reference": "ref2", "pct_uncertainty": 0.2, "uncertainty_notes": "None", "values": [ 2.0, -0.002 ] },
                    "surface_tension": { "range": [ 500.0, 750.0 ], "reference": "ref2", "pct_uncertainty": 0.2, "uncertainty_notes": "None", "values": [ 2.0, -0.004 ] }
                }
    }}}})test_data";

  Default_Data_Store d;
  std::istringstream in(scrap);
  d.from_json(in);

  // Verify Data was loaded (critical)
  std::vector<std::string> check_names = {"S1", "S2"};
  size_t dSize = 2;
  ASSERT_EQ(dSize, d.size());
  ASSERT_TRUE(d.valid(0));
  ASSERT_TRUE(d.valid(1));
  ASSERT_TRUE(!d.valid(2));
  ASSERT_TRUE(d.valid(check_names[0]));
  ASSERT_TRUE(!d.valid(check_names[1]));
  ASSERT_TRUE(d.valid(check_names));

  // Check that the data id interface works (critical)
  size_t tstID = d.name_to_id(check_names[0]);
  size_t refID = 0;
  ASSERT_EQ(refID, tstID);
  ASSERT_EQ(refID, d.names_to_id({check_names[0]}));
  EXPECT_EQ(1, d.names_to_id(check_names));

  // Check Data directly from data store, i.e. using ID and recID
  size_t nConstituents = d.constituent_count(refID);
  size_t refSize = 1;
  ASSERT_EQ(refSize, nConstituents);
  std::vector<std::string> tstNames = d.names(refID);
  EXPECT_EQ(tstNames[0], "S1");

  // Testing the first record of a given salt
  size_t tstRecID = 0;
  EXPECT_DOUBLE_EQ(500, d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1.00, d.melt_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.melt_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1000, d.boil(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.0, d.boil_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.boil_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.01, d.rho_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.rho_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.01, d.mu_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.mu_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.2, d.k_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.k_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.015, d.cp_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.cp_ref(refID, tstRecID));
  for (size_t i = 0; i < tVecK.size(); ++i) {
    // Density implements a linear model, A-(B*T)
    EXPECT_FLOAT_EQ(1.0 - (0.002 * tVecK[i]), d.rho(refID, tstRecID, tVecK[i]));
    // this item uses A*exp( B / R*T) where R is the molar gas constant constant
    EXPECT_FLOAT_EQ(0.50 * exp(2.0E+4 / (mGas_const * tVecK[i])),
                    d.mu(refID, tstRecID, tVecK[i]));
    // Thermal conductivity implements a linear model, A+(B*T)
    EXPECT_FLOAT_EQ(1.8 + (-3.9E-04 * tVecK[i]),
                    d.k(refID, tstRecID, tVecK[i]));
    // Heat capacity uses a degree 4 polynomial only the scalar portion is used
    // here
    EXPECT_FLOAT_EQ(6.5E01, d.cp(refID, tstRecID, tVecK[i]));
    // surface tension implements a linear model, A-(B*T)
    EXPECT_FLOAT_EQ(1.0 - (0.002 * tVecK[i]),
                    d.surfaceTension(refID, tstRecID, tVecK[i]));
    // h(t) = A * (T - Tmelt);  Tmelt:=500
    double ht = 6.5E01 * (tVecK[i] - 500);
    EXPECT_FLOAT_EQ(ht, d.h_t(refID, tstRecID, tVecK[i]));
    EXPECT_FLOAT_EQ(tVecK[i], d.t_h(refID, tstRecID, ht));

    // Check Temperature/Enthalpy interfaces are consistent
    EXPECT_FLOAT_EQ(d.rho(refID, tstRecID, tVecK[i]),
                    d.rho_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.mu(refID, tstRecID, tVecK[i]),
                    d.mu_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.k(refID, tstRecID, tVecK[i]), d.k_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.cp(refID, tstRecID, tVecK[i]),
                    d.cp_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.surfaceTension(refID, tstRecID, tVecK[i]),
                    d.surfaceTension_h(refID, tstRecID, ht));
  }

  // Check Data via views
  Data_Store::View v = d.setView({"S1"}, {1.0});

  // basic view info
  ASSERT_TRUE(!v.nullView());
  ASSERT_TRUE(v.d != nullptr);
  EXPECT_EQ(d.constituent_count(refID), v.constituent_count());
  EXPECT_EQ(refID, v.id);
  EXPECT_EQ(tstRecID, v.rec_id);

  EXPECT_DOUBLE_EQ(v.melt(), d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(v.boil(), d.boil(refID, tstRecID));

  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, tstRecID);

  // Check consistency from temperature to enthalpy
  compareTemp_to_Enthalpy(v);
}

//---------------------------------------------------------------------------//
// Simplest test case...load from string literal check unary salt
TEST(default_data_store, load_testData_literal) {
  // molar gas constant https://physics.nist.gov/cgi-bin/cuu/Value?r; accessed
  // 10Feb2021
  static constexpr double mGas_const = 8.314462618;
  // Generate some scrap data
  static const char *scrap = R"test_data(


    S1   ,1.0,Pure Salt,500.00,1.00,ref1,1000.00,0.0,ref1,1.0, 0.002,500-1000,1.00,ref1,0.50,2.0E+04,0.0,0.0,0.0,500-1000,1.00,ref1,1.8E+00,-3.9E-04,500-1000,20.00,ref1,6.5E+01,0.0,0.0,0.0,1.50,ref1,1.0, 0.002,500-1000,1.00,ref1
    S1-S2,2.0,0.36-0.64,500.00,0.50,ref2,1000.00,0.0,ref2,2.0,-0.002,500-750 ,0.20,ref2,1.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,1.2E+00,-2.8E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref2,2.0,-0.002,500-750 ,0.20,ref2
    )test_data";

  // Set-up work
  Default_Data_Store d;
  std::istringstream in(scrap);
  d.load(in);

  // Verify Data was loaded (critical)
  std::vector<std::string> check_names = {"S1", "S2"};
  size_t dSize = 2;
  ASSERT_EQ(dSize, d.size());
  ASSERT_TRUE(d.valid(0));
  ASSERT_TRUE(d.valid(1));
  ASSERT_TRUE(!d.valid(2));
  ASSERT_TRUE(d.valid(check_names[0]));
  ASSERT_TRUE(!d.valid(check_names[1]));
  ASSERT_TRUE(d.valid(check_names));

  // Check that the data id interface works (critical)
  size_t tstID = d.name_to_id(check_names[0]);
  size_t refID = 0;
  ASSERT_EQ(refID, tstID);
  ASSERT_EQ(refID, d.names_to_id({check_names[0]}));
  EXPECT_EQ(1, d.names_to_id(check_names));

  // Check Data directly from data store, i.e. using ID and recID
  size_t nConstituents = d.constituent_count(refID);
  size_t refSize = 1;
  ASSERT_EQ(refSize, nConstituents);
  std::vector<std::string> tstNames = d.names(refID);
  EXPECT_EQ(tstNames[0], "S1");

  // Testing the first record of a given salt
  size_t tstRecID = 0;
  EXPECT_DOUBLE_EQ(500, d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1.00, d.melt_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.melt_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1000, d.boil(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.0, d.boil_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.boil_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.01, d.rho_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.rho_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.01, d.mu_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.mu_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.2, d.k_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.k_ref(refID, tstRecID));
  EXPECT_DOUBLE_EQ(0.015, d.cp_unc(refID, tstRecID));
  ASSERT_EQ("ref1", d.cp_ref(refID, tstRecID));
  for (size_t i = 0; i < tVecK.size(); ++i) {
    // Density implements a linear model, A-(B*T)
    EXPECT_FLOAT_EQ(1.0 - (0.002 * tVecK[i]), d.rho(refID, tstRecID, tVecK[i]));
    // this item uses A*exp( B / R*T) where R is the molar gas constant constant
    EXPECT_FLOAT_EQ(0.50 * exp(2.0E+4 / (mGas_const * tVecK[i])),
                    d.mu(refID, tstRecID, tVecK[i]));
    // Thermal conductivity implements a linear model, A+(B*T)
    EXPECT_FLOAT_EQ(1.8 + (-3.9E-04 * tVecK[i]),
                    d.k(refID, tstRecID, tVecK[i]));
    // Heat capacity uses a degree 4 polynomial only the scalar portion is used
    // here
    EXPECT_FLOAT_EQ(6.5E01, d.cp(refID, tstRecID, tVecK[i]));
    // surface tension implements a linear model, A-(B*T)
    EXPECT_FLOAT_EQ(1.0 - (0.002 * tVecK[i]),
                    d.surfaceTension(refID, tstRecID, tVecK[i]));
    // h(t) = A * (T - Tmelt);  Tmelt:=500
    double ht = 6.5E01 * (tVecK[i] - 500);
    EXPECT_FLOAT_EQ(ht, d.h_t(refID, tstRecID, tVecK[i]));
    EXPECT_FLOAT_EQ(tVecK[i], d.t_h(refID, tstRecID, ht));

    // Check Temperature/Enthalpy interfaces are consistent
    EXPECT_FLOAT_EQ(d.rho(refID, tstRecID, tVecK[i]),
                    d.rho_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.mu(refID, tstRecID, tVecK[i]),
                    d.mu_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.k(refID, tstRecID, tVecK[i]), d.k_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.cp(refID, tstRecID, tVecK[i]),
                    d.cp_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.surfaceTension(refID, tstRecID, tVecK[i]),
                    d.surfaceTension_h(refID, tstRecID, ht));
  }

  // Check Data via views
  Data_Store::View v = d.setView({"S1"}, {1.0});

  // basic view info
  ASSERT_TRUE(!v.nullView());
  ASSERT_TRUE(v.d != nullptr);
  EXPECT_EQ(d.constituent_count(refID), v.constituent_count());
  EXPECT_EQ(refID, v.id);
  EXPECT_EQ(tstRecID, v.rec_id);

  EXPECT_DOUBLE_EQ(v.melt(), d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(v.boil(), d.boil(refID, tstRecID));

  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, tstRecID);

  // Check consistency from temperature to enthalpy
  compareTemp_to_Enthalpy(v);
}

//---------------------------------------------------------------------------//
// Test using binary salt permutations
TEST(default_data_store, load_testData_binary) {
  // molar gas constant https://physics.nist.gov/cgi-bin/cuu/Value?r; accessed
  // 10Feb2021
  static constexpr double mGas_const = 8.314462618;
  // Generate some scrap data
  static const char *scrap = R"test_data(


    S1      ,1.0,Pure Salt     ,500.00,1.00,ref1,1000.00,0.0,ref1,1.0, 0.002,500-1000,1.00,ref1,0.50,2.0E+04,0.0,0.0,0.0,500-1000,1.00,ref1,1.8E+00,-3.9E-04,500-1000,20.00,ref1,6.5E+01,0.0,0.0,0.0,1.50,ref1,----,----,----,----,----
    S1-S2   ,2.0,0.36-0.64     ,500.00,0.50,ref2,1000.00,0.0,ref2,2.0,-0.002,500-750 ,0.20,ref2,1.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,1.2E+00,-2.8E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref1,----,----,----,----,----
    S1-S2   ,2.0,0.64-0.36     ,550.00,0.50,ref2,1500.00,0.0,ref2,3.0,-0.004,500-750 ,0.20,ref2,3.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,2.2E+00,-2.5E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref1,----,----,----,----,----
    S1-S2   ,2.0,0.50-0.50     ,600.00,0.50,ref2,2000.00,0.0,ref2,4.0,-0.006,500-750 ,0.20,ref2,2.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,1.7E+00,-1.2E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref1,----,----,----,----,----
    S1-S2-S3,4.0,0.23-0.50-0.27,131.20,0.50,ref3,1771.00,0.0,ref3,1.5, 0.002,100-400 ,1.00,ref3,1.68,2.3E+04,0.0,0.0,0.0,141-1327,2.00,ref3,8.5E-01,-2.4E-04,129-1800,20.00,ref3,7.6E+01,0.0,0.0,0.0,1.00,ref1,----,----,----,----,----
    )test_data";

  // Set-up work
  Default_Data_Store d;
  std::istringstream in(scrap);
  d.load(in);

  // Verify Data was loaded (critical)
  size_t dSize = 3;
  ASSERT_EQ(dSize, d.size());
  ASSERT_TRUE(d.valid(0));
  ASSERT_TRUE(d.valid(1));
  ASSERT_TRUE(d.valid(2));
  ASSERT_TRUE(!d.valid(3));

  // Check that the data id interface works (critical)
  size_t tstID = d.names_to_id({"S1", "S2"});
  size_t refID = 1;
  ASSERT_EQ(refID, tstID);

  // Check Data directly from data store, i.e. using ID and recID
  size_t nConstituents = d.constituent_count(refID);
  size_t refSize = 2;
  ASSERT_EQ(refSize, nConstituents);
  std::vector<std::string> tstNames = d.names(refID);
  EXPECT_EQ(tstNames[0], "S1");
  EXPECT_EQ(tstNames[1], "S2");

  // Testing the first record of a given salt
  size_t tstRecID = 0;
  EXPECT_DOUBLE_EQ(500, d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(1000, d.boil(refID, tstRecID));

  for (size_t i = 0; i < tVecK.size(); ++i) {
    // Density implements a linear model, A-(B*T)
    EXPECT_FLOAT_EQ(2.0 - (-0.002 * tVecK[i]),
                    d.rho(refID, tstRecID, tVecK[i]));
    // this item uses A*exp( B / R*T) where R is the molar gas constant constant
    EXPECT_FLOAT_EQ(1.25 * exp(2.0E+4 / (mGas_const * tVecK[i])),
                    d.mu(refID, tstRecID, tVecK[i]));
    // Thermal conductivity implements a linear model, A+(B*T)
    EXPECT_FLOAT_EQ(1.2 + (-2.8E-04 * tVecK[i]),
                    d.k(refID, tstRecID, tVecK[i]));
    // Heat capacity uses a degree 4 polynomial only the scalar portion is used
    // here
    EXPECT_FLOAT_EQ(6.8E01, d.cp(refID, tstRecID, tVecK[i]));
    // h(t) = A * (T - Tmelt);  Tmelt:=500
    double ht = 6.8E01 * (tVecK[i] - 500);
    EXPECT_FLOAT_EQ(ht, d.h_t(refID, tstRecID, tVecK[i]));
    EXPECT_FLOAT_EQ(tVecK[i], d.t_h(refID, tstRecID, ht));

    // Check Temperature/Enthalpy interfaces are consistent
    EXPECT_FLOAT_EQ(d.rho(refID, tstRecID, tVecK[i]),
                    d.rho_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.mu(refID, tstRecID, tVecK[i]),
                    d.mu_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.k(refID, tstRecID, tVecK[i]), d.k_h(refID, tstRecID, ht));
    EXPECT_FLOAT_EQ(d.cp(refID, tstRecID, tVecK[i]),
                    d.cp_h(refID, tstRecID, ht));
  }

  // Check Data via views
  Data_Store::View v = d.setView({"S1", "S2"}, {0.36, 0.64});

  // basic view info
  ASSERT_TRUE(!v.nullView());
  ASSERT_TRUE(v.d != nullptr);
  EXPECT_EQ(d.constituent_count(refID), v.constituent_count());
  EXPECT_EQ(refID, v.id);
  EXPECT_EQ(tstRecID, v.rec_id);

  EXPECT_DOUBLE_EQ(v.melt(), d.melt(refID, tstRecID));
  EXPECT_DOUBLE_EQ(v.boil(), d.boil(refID, tstRecID));

  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, tstRecID);

  // Check consistency from temperature to enthalpy
  compareTemp_to_Enthalpy(v);

  // Check Data via views
  v = d.setView({"S1", "S2"}, {0.64, 0.36});
  EXPECT_DOUBLE_EQ(550, v.melt());
  EXPECT_DOUBLE_EQ(1500, v.boil());
  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, 1);

  v = d.setView({"S1", "S2"}, {0.5, 0.5});
  EXPECT_DOUBLE_EQ(600, v.melt());
  EXPECT_DOUBLE_EQ(2000, v.boil());
  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, 2);

  // Try something in between mole_percents
  Data_Store::View v1 = d.setView({"S1", "S2"}, {0.565, 0.435});
  EXPECT_DOUBLE_EQ(600, v.melt());
  EXPECT_DOUBLE_EQ(2000, v.boil());
  // Check consistency from view to data store
  compareView_to_DataStore(v, refID, 2);

  EXPECT_DOUBLE_EQ(v.melt(), v1.melt());
  EXPECT_DOUBLE_EQ(v.boil(), v1.boil());

  // Check consistency from view to data store
  for (size_t i = 0; i < tVecK.size(); ++i) {
    EXPECT_FLOAT_EQ(v.rho(tVecK[i]), v1.rho(tVecK[i]));
    EXPECT_FLOAT_EQ(v.mu(tVecK[i]), v1.mu(tVecK[i]));
    EXPECT_FLOAT_EQ(v.k(tVecK[i]), v1.k(tVecK[i]));
    EXPECT_FLOAT_EQ(v.cp(tVecK[i]), v1.cp(tVecK[i]));
    EXPECT_FLOAT_EQ(v.h_t(tVecK[i]), v1.h_t(tVecK[i]));
  }
}

//---------------------------------------------------------------------------//
// Test a compound from default data
TEST(default_data, FLiNaK_465_115_042) {
  std::istringstream in(tst_data);
  Default_Data_Store d;
  d.load(in);
  Thermophysical_Properties tp;
  ASSERT_TRUE(tp.initialize(&d));

  // Ensure that the composition set correctly
  ASSERT_TRUE(tp.setComposition({"LiF", "NaF", "KF"}, {0.465, 0.115, 0.42}));

  // Test Density
  std::vector<double> tks = {700.0, 750.0, 800.0, 850.0, 860.0, 1000.0};
  // Expected calculated values (note these have been rounded)
  std::vector<double> rho_calc_ref = {2.1425,  2.1113,  2.0801,
                                      2.04890, 2.04266, 1.9553};
  for (size_t i = 0; i < tks.size(); ++i) {
    double t_k = tks[i];
    EXPECT_NEAR(rho_calc_ref[i], tp.rho(t_k), 1e-3);
  }
  EXPECT_NEAR(tp.rho_h(tp.h_t(900)), tp.rho(900), 1e-4);
  EXPECT_FLOAT_EQ(tp.rho(900) * 1000.0, tp.rho_kgm3(900));
  tks.clear();

  // Test Heat Capacity
  // Using values from reference https://doi.org/10.1021/je00029a041
  tks = {700.0, 750.0, 800.0, 850.0, 860.0, 1000.0};
  // Expected calculated values (note these have been rounded)
  std::vector<double> cp_calc_ref = {71.0316, 73.2255, 75.4194,
                                     77.6133, 78.0521, 84.195};

  for (size_t i = 0; i < tks.size(); ++i) {
    double t_k = tks[i];
    EXPECT_NEAR(cp_calc_ref[i], tp.cp(t_k), 6e-3);
  }
  EXPECT_FLOAT_EQ(tp.cp(900) * (1000.0 / 41.2909), tp.cp_kg(900));
  EXPECT_NEAR(tp.cp_h(tp.h_t(1200)), tp.cp(1200), 4e-2);
  tks.clear();

  // Test Viscosity
  // Using values from reference https://doi.org/10.1021/je60084a007
  tks = {700.0, 822.45, 872.65, 973.45, 922.95, 770.15, 1000.0};
  // Expected calculated value
  std::vector<double> mu_calc_ref = {17.9814, 5.6296, 4.0862, 2.5436,
                                     3.1494,  8.5459, 2.3095};

  for (size_t i = 0; i < tks.size(); ++i) {
    double t_k = tks[i];
    EXPECT_NEAR(mu_calc_ref[i], tp.mu(t_k), 5e-2);
  }
  tks.clear();

  EXPECT_NEAR(tp.mu_h(tp.h_t(900)), tp.mu(900), 6e-4);

  // Test Thermal Conductivity
  // Using values from reference
  // https://doi.org/10.1016/j.ijheatmasstransfer.2015.07.042
  tks = {700.0, 773.0, 823.0, 873.0, 923.0, 973.0, 1000.0};
  // Expected calculated value
  std::vector<double> k_calc_ref = {0.56,   0.6549, 0.7199, 0.7849,
                                    0.8499, 0.9149, 0.95};

  for (size_t i = 0; i < tks.size(); ++i) {
    double t_k = tks[i];
    EXPECT_NEAR(k_calc_ref[i], tp.k(t_k), 5e-4);
  }
  EXPECT_NEAR(tp.k_h(tp.h_t(970)), tp.k(970), 1e-4);

  // Test temperature to enthalpy conversion
  EXPECT_NEAR(748.77152178380845, tp.t_h(1000), 1e-6);
  EXPECT_NEAR(tp.t_h(tp.h_t(970)), 970, 5e-2);
  EXPECT_NEAR(tp.t_h_kg(tp.h_t_kg(970)), 970, 5e-2);

  // Set an incomplete data sets to test validity functions
  tp.setComposition({"LiF", "BeF2", "ThF4"}, {0.727, 0.157, 0.116});
  ASSERT_FALSE(tp.valid_rho());
  ASSERT_FALSE(tp.valid_k());
  ASSERT_FALSE(tp.valid_cp());
  ASSERT_TRUE(tp.valid_mu());
  tp.setComposition({"LiF", "BeF2", "ThF4"}, {0.6998, 0.1499, 0.1503});
  ASSERT_FALSE(tp.valid_mu());
}
