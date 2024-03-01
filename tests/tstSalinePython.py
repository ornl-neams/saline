import unittest
import tempfile
from SalinePy import Default_Data_Store, Thermophysical_Properties

#Temporarily add necessary test data
tst_data_rk = b"\n\
//Pure Salts\n\
//System      , id , Mol Mass , Mol Frac             , Melt(K) , Var(%) , Reference   , Boil(K) , Var(%) , Reference      , rho_a     , rho_b     , range(K)      , Var(%) , Reference     , mu1_a     , mu2_b     , mu2_a      , mu2_b      , mu2_c     , range(K)      , Var(%) , Reference    , k_a        , k_b        , range(K)     , Var(%) , Reference   , cp_a      , cp_b       , cp_c       , cp_d      , Var(%) , Reference\n\
LiF           , 1  , 25.9390  , 1.0                  , 1121.20 , 1.00   , Douglas1954 , 1943.00 , 0.00   , Ruff1922       , 2.371E+00 , 5.000E-04 , 1123.6-1367.5 , 1.00   , Hill1967      , 1.149E-01 , 2.699E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1133-1772     , 1.00   , Abe1981      , 1.882E+00  , -3.990E-04 , 1118.5-1900  , 20.00  , Gheribi2014 , 6.489E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1.50   , Douglas1954\n\
NaF           , 2  , 41.9882  , 1.0                  , 1268.00 , 0.50   , Cantor1961  , 1978.00 , 0.00   , Ruff1922       , 2.755E+00 , 6.360E-04 , 1273-1373     , 0.20   , Paucirova1970 , 1.196E-01 , 2.649E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1273-1373     , 1.00   , Brockner1979 , 1.259E+00  , -2.800E-04 , 1268.15-1800 , 20.00  , Gheribi2014 , 6.862E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.10   , O'Brien1957\n\
KF            , 3  , 58.0967  , 1.0                  , 1131.20 , 0.50   , Johnson1958 , 1771.00 , 0.00   , Ruff1922       , 2.646E+00 , 6.515E-04 , 1154.2-1310.2 , 1.00   , Yaffe1956     , 1.068E-01 , 2.378E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1141.2-1327.6 , 2.00   , Janz1988     , 8.552E-01  , -2.540E-04 , 1129.15-1800 , 20.00  , Gheribi2014 , 7.060E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 18.00  , Beilmann2013\n\
BeF2          , 4  , 47.0100  , 1.0                  , 821.15  , 1.00   , Thoma1960   , 1442.00 , 0.00   , Cantor1965     , 1.972E+00 , 1.450E-05 , 1073-1123     , 0.50   , Cantor1969    , 0.000E+00 , 0.000E+00 , -8.119E+00 , 1.149E+04  , 6.390E+05 , 846.9-1252.2  , 3.00   , Moynihan1968 , 8.007E-01  , -2.120E-06 , 1070.15-0.0  , 20.00  , Gheribi2014 , 1.027E+02 , -1.539E-03 , -1.565E+07 , 3.000E-09 , 0.00   , Glushko1994\n\
ThF4          , 5  , 308.0300 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 7.080E+00 , 0.000E+00 , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----\n\
UF3           , 6  , 295.0200 , 1.0                  , 1768.15 , 20.00  , D'Eye1957   , 2550.00 , 0.00   , Glushko1994    , 0.000E+00 , 0.000E+00 , 0.0-0.0       , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 1.300E+02 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 5.00   , Beilmann2013a\n\
ZrF4          , 7  , 167.2100 , 1.0                  , 0.00    , 0.00   , ----        , 0.00    , 0.00   , ----           , 5.368E+00 , 5.3677    , 0-0           , 0.00   , ----          , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.00   , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.00   , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.00   , ----\n\
LiF-NaF-KF    , 8  , 41.2909  , 0.465-0.115-0.42     , 735.00  , 2.00   , Rogers1982  , 1843.15 , 0.0    , Williams 2006b , 2.579E+00 , 6.240E-04 , 933-1163      , 1.00   , Cibulkova2006 , 0.000E+00 , 0.000E+00 , 2.130E-01  , -1.200E+03 , 1.350E+06 , 770-970       , 2.00   , Toerklep1980 , -3.500E-01 , 1.300E-03  , 773-973      , 3.50   , An2015      , 4.032E+01 , 4.388E-02  , 0.000E+00  , 0.000E+00 , 2.00   , Rogers1982\n\
LiF-BeF2-ThF4 , 9  , 47.9244  , 0.7011-0.2388-0.0601 , 828.30  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 3.295E+00 , 6.707E-04 , 828.3-980.6   , 1.00   , Cantor1973    , 6.602E-02 , 3.642E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 799-926       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----\n\
LiF-BeF2-ThF4 , 10 , 63.5179  , 0.7006-0.1796-0.1198 , 806.40  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.044E+00 , 8.064E-04 , 806.4-1014.4  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----\n\
LiF-BeF2-ThF4 , 11 , 71.4958  , 0.6998-0.1499-0.1503 , 816.60  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 4.441E+00 , 9.526E-04 , 816.6-1022.7  , 1.00   , Cantor1973    , 0.000E+00 , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0-0.0       , 0.0    , ----         , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----\n\
LiF-BeF2-ThF4 , 12 , 61.9697  , 0.727-0.157-0.116    , 826.20  , 0.00   , Cantor1973  , 0.0     , 0.0    , ----           , 0.0       , 0.0       , 0.0-0.0       , 0.0    , ----          , 1.094E-01 , 3.402E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 826-946       , 15.00  , Cantor1973   , 0.000E+00  , 0.000E+00  , 0.0-0.0      , 0.0    , ----        , 0.000E+00 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.0    , ----\n"

class FixturesTest(unittest.TestCase):

    def test_LiFNaFKF_465_115_042(self):
        # Initialize
        tstDat = tempfile.NamedTemporaryFile(delete=False)
        tstDat.write(tst_data_rk)
        tstDat.close()

        self.d = Default_Data_Store()
        self.d.load(tstDat.name)
        self.tp = Thermophysical_Properties()
        self.assertTrue(self.tp.initialize(self.d))

        # Set up a composition
        self.assertTrue(self.tp.setComposition(["LiF","NaF","KF"],[0.465,0.115,0.42]))

        ## Density
        self.assertAlmostEqual(2.1425, self.tp.rho(700),3)
        self.assertAlmostEqual(self.tp.rho_h(self.tp.h_t(900)),self.tp.rho(900),delta=1.e-4)

        ## Heat Capacity
        # Set temperature range
        tks = [700.0, 750.0, 800.0, 850.0, 860.0, 1000.0]
        # Expected calculated values (note these have been rounded)
        cp_calc_ref = [71.0316, 73.2255, 75.4194, 77.6133, 78.0521, 84.195]

        for i,t_k in enumerate(tks):
            self.assertAlmostEqual(cp_calc_ref[i],self.tp.cp(t_k),delta=6.e-3)
        self.assertAlmostEqual(self.tp.cp_h(self.tp.h_t(1200)),self.tp.cp(1200),delta=4.e-2)

        ## Viscosity
        tks = [700.0, 822.45, 872.65, 973.45, 922.95, 770.15, 1000.0]
        # Expected calculated value
        mu_calc_ref = [17.9814 , 5.6296, 4.0862, 2.5436, 3.1494, 8.5459, 2.3095]

        for i,t_k in enumerate(tks):
            self.assertAlmostEqual(mu_calc_ref[i],self.tp.mu(t_k),delta=5.e-2)
        self.assertAlmostEqual(self.tp.mu_h(self.tp.h_t(900)),self.tp.mu(900),delta=6.e-2)

        ## Conductivity
        tks = [700.0, 773.0,  823.0,  873.0,  923.0,  973.0, 1000.0]
        # Expected calculated value
        k_calc_ref = [0.56, 0.6549, 0.7199, 0.7849, 0.8499, 0.9149, 0.95]

        for i,t_k in enumerate(tks):
            self.assertAlmostEqual(k_calc_ref[i],self.tp.k(t_k),delta=5.e-4)

        self.assertAlmostEqual(self.tp.k_h(self.tp.h_t(970)),self.tp.k(970),delta=1.e-4)
        self.assertAlmostEqual(748.77152178380845, self.tp.t_h(1000),delta=1.e-6)
        self.assertAlmostEqual(self.tp.t_h(self.tp.h_t(970)),970,delta=5.e-2)

if __name__ == '__main__':
    unittest.main(verbosity=2)
