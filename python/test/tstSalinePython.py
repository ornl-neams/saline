import unittest
from SalinePy import Default_Data_Store, Thermophysical_Properties

class FixturesTest(unittest.TestCase):

    def test_LiFNaFKF_465_115_042(self):
        # Initialize
        self.d = Default_Data_Store()
        self.d.load()
        self.tp = Thermophysical_Properties()
        self.assertTrue(self.tp.initialize(self.d))

        # Set up a composition
        self.assertTrue(self.tp.setComposition(["LiF","NaF","KF"],[0.465,0.115,0.42]))

        ## Density
        self.assertAlmostEqual(2.01770, self.tp.rho(900),5)
        self.assertAlmostEqual(self.tp.rho_h(self.tp.h_t(900)),self.tp.rho(900),4)

        ## Heat Capacity
        # Set temperature range
        tks = [750, 800, 850, 860]
        # NOTE reference values are {17.50, 18.03, 18.55, 18.60} cal mol^-1 K^-1, but we use J mol^-1 K^-1
        cp_exp = [73.22, 75.43752, 75.43752, 77.8224]
        # Expected calculated values (note these have been rounded)
        cp_calc_ref = [73.2255, 75.4194, 77.6133, 78.0521]

        for i,t_k in enumerate(tks):
            self.assertAlmostEqual(cp_calc_ref[i],self.tp.cp(t_k),4)
        self.assertAlmostEqual(self.tp.cp_h(self.tp.h_t(1200)),self.tp.cp(1200),2)

        ## Viscosity
        tks = [822.45, 872.65, 973.45, 922.95, 770.15]
        # Experimental values
        mu_exp =  [5.615,  4.097,  2.535,  3.156,  8.551]
        # Expected calculated value
        mu_calc_ref = [5.6296, 4.0862, 2.5436, 3.1494, 8.5459]

        for i,t_k in enumerate(tks):
            self.assertAlmostEqual(mu_calc_ref[i],self.tp.mu(t_k),4)
        self.assertAlmostEqual(self.tp.mu_h(self.tp.h_t(900)),self.tp.mu(900),3)

        ## Conductivity
        tks = [773.0,  823.0,  873.0,  923.0,  973.0]
        # Experimental values
        k_exp = [0.6520, 0.7730, 0.7720, 0.8320, 0.9270]
        # Expected calculated value
        k_calc_ref = [0.6549, 0.7199, 0.7849, 0.8499, 0.9149]

        for i,t_k in enumerate(tks):
            self.assertAlmostEqual(k_calc_ref[i],self.tp.k(t_k),4)

        self.assertAlmostEqual(self.tp.k_h(self.tp.h_t(970)),self.tp.k(970),3)
        self.assertAlmostEqual(748.77235618, self.tp.t_h(1000),5)
        self.assertAlmostEqual(self.tp.t_h(self.tp.h_t(970)),970,1)

if __name__ == '__main__':
    unittest.main(verbosity=2)
