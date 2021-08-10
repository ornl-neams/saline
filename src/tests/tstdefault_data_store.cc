#include "gtest/gtest.h"

#include "../default_data_store.hh"
#include "../thermophysical_properties.hh"
#include "../utils.hh"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace saline;

//Temperatures in Kelvin
static std::vector<double> tVecK = {500.00,750.00,800.00,850.00,900.00,950.00,1000.00};

//---------------------------------------------------------------------------//
// A helper function to consolidate some repeated calls
void compareView_to_DataStore(Data_Store::View& v, Data_Store::Id compId, Data_Store::Id recId)
{
    EXPECT_DOUBLE_EQ(v.melt(),v.d->melt(compId,recId));
    EXPECT_DOUBLE_EQ(v.boil(),v.d->boil(compId,recId));

    // Check consistency from view to data store
    for(size_t i=0; i<tVecK.size(); ++i)
    {
        EXPECT_FLOAT_EQ(v.rho(tVecK[i]),v.d->rho(compId,recId,tVecK[i]));
        EXPECT_FLOAT_EQ(v.mu( tVecK[i]),v.d->mu( compId,recId,tVecK[i]));
        EXPECT_FLOAT_EQ(v.k(  tVecK[i]),v.d->k(  compId,recId,tVecK[i]));
        EXPECT_FLOAT_EQ(v.cp( tVecK[i]),v.d->cp( compId,recId,tVecK[i]));
        EXPECT_FLOAT_EQ(v.h_t(tVecK[i]),v.d->h_t(compId,recId,tVecK[i]));
    }
}

// A helper function to consolidate some repeated calls
void compareTemp_to_Enthalpy(Data_Store::View& v)
{
    for(size_t i=0; i<tVecK.size(); ++i)
    {
        double ht = v.h_t(tVecK[i]);
        EXPECT_FLOAT_EQ(      tVecK[i], v.t_h(  ht));
        EXPECT_FLOAT_EQ(v.rho(tVecK[i]),v.rho_h(ht));
        EXPECT_FLOAT_EQ(v.mu( tVecK[i]),v.mu_h( ht));
        EXPECT_FLOAT_EQ(v.k(  tVecK[i]),v.k_h(  ht));
        EXPECT_FLOAT_EQ(v.cp( tVecK[i]),v.cp_h( ht));
    }

}

//---------------------------------------------------------------------------//
// Simplest test case...load from string literal check unary salt
TEST(default_data_store,load_testData_literal)
{
    // molar gas constant https://physics.nist.gov/cgi-bin/cuu/Value?r; accessed 10Feb2021
    static constexpr double mGas_const = 8.314462618;
    // Generate some scrap data
    static const char* scrap = R"test_data(
    S1   ,Pure Salt,1.0,500.00,1.00,ref1,1000.00,0.0,ref1,1.0, 0.002,500-1000,1.00,ref1,0.50,2.0E+04,0.0,0.0,0.0,500-1000,1.00,ref1,1.8E+00,-3.9E-04,500-1000,20.00,ref1,6.5E+01,0.0,0.0,0.0,1.50,ref1
    S1-S2,0.36-0.64,2.0,500.00,0.50,ref2,1000.00,0.0,ref2,2.0,-0.002,500-750 ,0.20,ref2,1.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,1.2E+00,-2.8E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref2
    )test_data";

    // Set-up work
    Default_Data_Store d;
    std::istringstream in(scrap);
    d.load(in);

    // Verify Data was loaded (critical)
    size_t dSize = 2;
    ASSERT_EQ(dSize,d.size());
    ASSERT_TRUE(d.valid(0));
    ASSERT_TRUE(d.valid(1));
    ASSERT_TRUE(!d.valid(2));

    // Check that the data id interface works (critical)
    size_t tstID = d.name_to_id("S1");
    size_t refID = 0;
    ASSERT_EQ(refID,tstID);
    ASSERT_EQ(refID,d.names_to_id({"S1"}));
    EXPECT_EQ(1,d.names_to_id({"S1","S2"}));

    //Check Data directly from data store, i.e. using ID and recID
    size_t nConstituents = d.constituent_count(refID);
    size_t refSize = 1;
    ASSERT_EQ(refSize,nConstituents);
    std::vector<std::string> tstNames = d.names(refID);
    EXPECT_EQ(tstNames[0],"S1");

    // Testing the first record of a given salt
    size_t tstRecID = 0;
    EXPECT_DOUBLE_EQ(500,d.melt(refID,tstRecID));
    EXPECT_DOUBLE_EQ(1000,d.boil(refID,tstRecID));
    for(size_t i=0; i<tVecK.size(); ++i)
    {
        // Density implements a linear model, A-(B*T)
        EXPECT_FLOAT_EQ(1.0-(0.002*tVecK[i]),d.rho(refID,tstRecID,tVecK[i]));
        // this item uses A*exp( B / R*T) where R is the molar gas constant constant
        EXPECT_FLOAT_EQ(0.50*exp(2.0E+4/(mGas_const*tVecK[i])),d.mu(refID,tstRecID,tVecK[i]));
        // Thermal conductivity implements a linear model, A+(B*T)
        EXPECT_FLOAT_EQ(1.8+(-3.9E-04*tVecK[i]),d.k(refID,tstRecID,tVecK[i]));
        // Heat capacity uses a degree 4 polynomial only the scalar portion is used here
        EXPECT_FLOAT_EQ(6.5E01,d.cp(refID,tstRecID,tVecK[i]));
        // h(t) = A * (T - Tmelt);  Tmelt:=500
        double ht = 6.5E01*(tVecK[i]-500);
        EXPECT_FLOAT_EQ(ht,d.h_t(refID,tstRecID,tVecK[i]));
        EXPECT_FLOAT_EQ(tVecK[i],d.t_h(refID,tstRecID,ht));

        //Check Temperature/Enthalpy interfaces are consistent
        EXPECT_FLOAT_EQ(d.rho(refID,tstRecID,tVecK[i]),d.rho_h(refID,tstRecID,ht));
        EXPECT_FLOAT_EQ(d.mu(refID,tstRecID,tVecK[i]),d.mu_h(refID,tstRecID,ht));
        EXPECT_FLOAT_EQ(d.k(refID,tstRecID,tVecK[i]),d.k_h(refID,tstRecID,ht));
        EXPECT_FLOAT_EQ(d.cp(refID,tstRecID,tVecK[i]),d.cp_h(refID,tstRecID,ht));
    }

    // Check Data via views
    Data_Store::View v = d.setView({"S1"},{1.0});

    // basic view info
    ASSERT_TRUE(!v.null());
    ASSERT_TRUE(v.d != nullptr);
    EXPECT_EQ(d.constituent_count(refID),v.constituent_count());
    EXPECT_EQ(refID,v.id);
    EXPECT_EQ(tstRecID,v.rec_id);

    std::vector<double> testMP = {1.0};
    ASSERT_EQ(testMP.size(),v.mole_percents.size());
    for( size_t i=0; i<testMP.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(testMP[i],v.mole_percents[i]);
    }

    EXPECT_DOUBLE_EQ(v.melt(),d.melt(refID,tstRecID));
    EXPECT_DOUBLE_EQ(v.boil(),d.boil(refID,tstRecID));

    // Check consistency from view to data store
    compareView_to_DataStore(v,refID,tstRecID);

    // Check consistency from temperature to enthalpy
    compareTemp_to_Enthalpy(v);
}

//---------------------------------------------------------------------------//
// Test using binary salt permutations
TEST(default_data_store,load_testData_binary)
{
    // molar gas constant https://physics.nist.gov/cgi-bin/cuu/Value?r; accessed 10Feb2021
    static constexpr double mGas_const = 8.314462618;
    // Generate some scrap data
    static const char* scrap = R"test_data(
    S1      ,Pure Salt     ,1.0,500.00,1.00,ref1,1000.00,0.0,ref1,1.0, 0.002,500-1000,1.00,ref1,0.50,2.0E+04,0.0,0.0,0.0,500-1000,1.00,ref1,1.8E+00,-3.9E-04,500-1000,20.00,ref1,6.5E+01,0.0,0.0,0.0,1.50,ref1
    S1-S2   ,0.36-0.64     ,2.0,500.00,0.50,ref2,1000.00,0.0,ref2,2.0,-0.002,500-750 ,0.20,ref2,1.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,1.2E+00,-2.8E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref2
    S1-S2   ,0.64-0.36     ,2.0,550.00,0.50,ref2,1500.00,0.0,ref2,3.0,-0.004,500-750 ,0.20,ref2,3.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,2.2E+00,-2.5E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref2
    S1-S2   ,0.50-0.50     ,2.0,600.00,0.50,ref2,2000.00,0.0,ref2,4.0,-0.006,500-750 ,0.20,ref2,2.25,2.0E+04,0.0,0.0,0.0,800-1000,1.00,ref2,1.7E+00,-1.2E-04,600-900 ,20.00,ref2,6.8E+01,0.0,0.0,0.0,0.10,ref2
    S1-S2-S3,0.23-0.50-0.27,4.0,131.20,0.50,ref3,1771.00,0.0,ref3,1.5, 0.002,100-400 ,1.00,ref3,1.68,2.3E+04,0.0,0.0,0.0,141-1327,2.00,ref3,8.5E-01,-2.4E-04,129-1800,20.00,ref3,7.6E+01,0.0,0.0,0.0,1.00,ref3
    )test_data";

    // Set-up work
    Default_Data_Store d;
    std::istringstream in(scrap);
    d.load(in);

    // Verify Data was loaded (critical)
    size_t dSize = 3;
    ASSERT_EQ(dSize,d.size());
    ASSERT_TRUE(d.valid(0));
    ASSERT_TRUE(d.valid(1));
    ASSERT_TRUE(d.valid(2));
    ASSERT_TRUE(!d.valid(3));

    // Check that the data id interface works (critical)
    size_t tstID = d.names_to_id({"S1","S2"});
    size_t refID = 1;
    ASSERT_EQ(refID,tstID);

    //Check Data directly from data store, i.e. using ID and recID
    size_t nConstituents = d.constituent_count(refID);
    size_t refSize = 2;
    ASSERT_EQ(refSize,nConstituents);
    std::vector<std::string> tstNames = d.names(refID);
    EXPECT_EQ(tstNames[0],"S1");
    EXPECT_EQ(tstNames[1],"S2");

    // Testing the first record of a given salt
    size_t tstRecID = 0;
    EXPECT_DOUBLE_EQ( 500,d.melt(refID,tstRecID));
    EXPECT_DOUBLE_EQ(1000,d.boil(refID,tstRecID));

    for(size_t i=0; i<tVecK.size(); ++i)
    {
        // Density implements a linear model, A-(B*T)
        EXPECT_FLOAT_EQ(2.0-(-0.002*tVecK[i]),d.rho(refID,tstRecID,tVecK[i]));
        // this item uses A*exp( B / R*T) where R is the molar gas constant constant
        EXPECT_FLOAT_EQ(1.25*exp(2.0E+4/(mGas_const*tVecK[i])),d.mu(refID,tstRecID,tVecK[i]));
        // Thermal conductivity implements a linear model, A+(B*T)
        EXPECT_FLOAT_EQ(1.2+(-2.8E-04*tVecK[i]),d.k(refID,tstRecID,tVecK[i]));
        // Heat capacity uses a degree 4 polynomial only the scalar portion is used here
        EXPECT_FLOAT_EQ(6.8E01,d.cp(refID,tstRecID,tVecK[i]));
        // h(t) = A * (T - Tmelt);  Tmelt:=500
        double ht = 6.8E01*(tVecK[i]-500);
        EXPECT_FLOAT_EQ(ht,d.h_t(refID,tstRecID,tVecK[i]));
        EXPECT_FLOAT_EQ(tVecK[i],d.t_h(refID,tstRecID,ht));

        //Check Temperature/Enthalpy interfaces are consistent
        EXPECT_FLOAT_EQ(d.rho(refID,tstRecID,tVecK[i]),d.rho_h(refID,tstRecID,ht));
        EXPECT_FLOAT_EQ(d.mu(refID,tstRecID,tVecK[i]),d.mu_h(refID,tstRecID,ht));
        EXPECT_FLOAT_EQ(d.k(refID,tstRecID,tVecK[i]),d.k_h(refID,tstRecID,ht));
        EXPECT_FLOAT_EQ(d.cp(refID,tstRecID,tVecK[i]),d.cp_h(refID,tstRecID,ht));
    }

    // Check Data via views
    Data_Store::View v = d.setView({"S1","S2"},{0.36,0.64});

    // basic view info
    ASSERT_TRUE(!v.null());
    ASSERT_TRUE(v.d != nullptr);
    EXPECT_EQ(d.constituent_count(refID),v.constituent_count());
    EXPECT_EQ(refID,v.id);
    EXPECT_EQ(tstRecID,v.rec_id);

    std::vector<double> testMP = {0.36,0.64};
    ASSERT_EQ(testMP.size(),v.mole_percents.size());
    for( size_t i=0; i<testMP.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(testMP[i],v.mole_percents[i]);
    }
    EXPECT_DOUBLE_EQ(v.melt(),d.melt(refID,tstRecID));
    EXPECT_DOUBLE_EQ(v.boil(),d.boil(refID,tstRecID));

    // Check consistency from view to data store
    compareView_to_DataStore(v,refID,tstRecID);

    // Check consistency from temperature to enthalpy
    compareTemp_to_Enthalpy(v);

    // Check Data via views
    v = d.setView({"S1","S2"},{0.64,0.36});
    EXPECT_DOUBLE_EQ( 550,v.melt());
    EXPECT_DOUBLE_EQ(1500,v.boil());
    // Check consistency from view to data store
    compareView_to_DataStore(v,refID,1);

    v = d.setView({"S1","S2"},{0.5,0.5});
    EXPECT_DOUBLE_EQ( 600,v.melt());
    EXPECT_DOUBLE_EQ(2000,v.boil());
    // Check consistency from view to data store
    compareView_to_DataStore(v,refID,2);

    // Try something in between mole_percents
    Data_Store::View v1 = d.setView({"S1","S2"},{0.565,0.435});
    EXPECT_DOUBLE_EQ( 600,v.melt());
    EXPECT_DOUBLE_EQ(2000,v.boil());
    // Check consistency from view to data store
    compareView_to_DataStore(v,refID,2);

    EXPECT_DOUBLE_EQ(v.melt(),v1.melt());
    EXPECT_DOUBLE_EQ(v.boil(),v1.boil());

    // Check consistency from view to data store
    for(size_t i=0; i<tVecK.size(); ++i)
    {
        EXPECT_FLOAT_EQ(v.rho(tVecK[i]),v1.rho(tVecK[i]));
        EXPECT_FLOAT_EQ(v.mu( tVecK[i]),v1.mu( tVecK[i]));
        EXPECT_FLOAT_EQ(v.k(  tVecK[i]),v1.k(  tVecK[i]));
        EXPECT_FLOAT_EQ(v.cp( tVecK[i]),v1.cp( tVecK[i]));
        EXPECT_FLOAT_EQ(v.h_t(tVecK[i]),v1.h_t(tVecK[i]));
    }
}

//---------------------------------------------------------------------------//
// Test a compound from default data
TEST(default_data, FLiNaK_465_115_042)
{
    Default_Data_Store d; d.load();
    Thermophysical_Properties tp;
    ASSERT_TRUE(tp.initialize(&d));

    // Ensure that the composition set correctly
    ASSERT_TRUE(tp.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42}));

    // Test Density
    std::vector<double> tks           = {700.0, 750.0, 800.0, 850.0, 860.0, 1000.0};
    // Expected calculated values (note these have been rounded)
    std::vector<double> rho_calc_ref  = {2.1425,2.1113,2.0801,2.04890,2.04266,1.9553};
    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(rho_calc_ref[i], tp.rho(t_k), 5e-4);
    }
    EXPECT_NEAR(tp.rho_h(tp.h_t(900)), tp.rho(900), 1e-4);
    tks.clear();

    // Test Heat Capacity
    // Using values from reference https://doi.org/10.1021/je00029a041
    tks           = {700.0, 750.0, 800.0, 850.0, 860.0, 1000.0};
    // Expected calculated values (note these have been rounded)
    std::vector<double> cp_calc_ref  = {71.0316, 73.2255, 75.4194, 77.6133, 78.0521, 84.195};

    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(cp_calc_ref[i], tp.cp(t_k), 5e-4);
    }
    EXPECT_NEAR(tp.cp_h(tp.h_t(1200)), tp.cp(1200), 4e-2);
    tks.clear();

    // Test Viscosity
    // Using values from reference https://doi.org/10.1021/je60084a007
    tks                              = {700.0, 822.45, 872.65, 973.45, 922.95, 770.15, 1000.0};
    // Expected calculated value
    std::vector<double> mu_calc_ref  = {17.9814 , 5.6296, 4.0862, 2.5436, 3.1494, 8.5459, 2.3095};

    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(mu_calc_ref[i], tp.mu(t_k), 5e-4);
    }
    tks.clear();

    EXPECT_NEAR(tp.mu_h(tp.h_t(900)), tp.mu(900), 6e-4);

    // Test Thermal Conductivity
    // Using values from reference https://doi.org/10.1016/j.ijheatmasstransfer.2015.07.042
    tks                              = {700.0, 773.0,  823.0,  873.0,  923.0,  973.0, 1000.0};
    // Expected calculated value
    std::vector<double> k_calc_ref  = {0.56, 0.6549, 0.7199, 0.7849, 0.8499, 0.9149, 0.95};

    for( size_t i = 0; i < tks.size(); ++i)
    {
        double t_k = tks[i];
        EXPECT_NEAR(k_calc_ref[i], tp.k(t_k), 5e-4);
    }
    EXPECT_NEAR(tp.k_h(tp.h_t(970)), tp.k(970), 1e-4);

    // Test temperature to enthalpy conversion
    EXPECT_NEAR(748.77235618, tp.t_h(1000), 1e-6);
    EXPECT_NEAR(tp.t_h(tp.h_t(970)),970,5e-2);
}
