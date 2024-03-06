#SalinePy
This provides the python interface to the Saline library. See Saline for details on basic features.

These wrappers are still very much in development and likely do not represent the final product.

## Usage
Saline is striving to implement test driven design so the unit tests will be the best source of examples.

Nevertheless a basic use is provided below. NOTE: the data file must be supplied by user.

```
import tempfile
from SalinePy import Default_Data_Store, Thermophysical_Properties

#Temporarily add necessary test data. For illustration only, the interface takes
#a path to a data file.
tst_data = b"\n\
//Pure Salts\n\
//System      , id , Mol Mass , Mol Frac             , Melt(K) , Var(%) , Reference   , Boil(K) , Var(%) , Reference      , rho_a     , rho_b     , range(K)      , Var(%) , Reference     , mu1_a     , mu2_b     , mu2_a      , mu2_b      , mu2_c     , range(K)      , Var(%) , Reference    , k_a        , k_b        , range(K)     , Var(%) , Reference   , cp_a      , cp_b       , cp_c       , cp_d      , Var(%) , Reference\n\
LiF           , 1  , 25.9390  , 1.0                  , 1121.20 , 1.00   , Douglas1954 , 1943.00 , 0.00   , Ruff1922       , 2.371E+00 , 5.000E-04 , 1123.6-1367.5 , 1.00   , Hill1967      , 1.149E-01 , 2.699E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1133-1772     , 1.00   , Abe1981      , 1.882E+00  , -3.990E-04 , 1118.5-1900  , 20.00  , Gheribi2014 , 6.489E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1.50   , Douglas1954\n\
NaF           , 2  , 41.9882  , 1.0                  , 1268.00 , 0.50   , Cantor1961  , 1978.00 , 0.00   , Ruff1922       , 2.755E+00 , 6.360E-04 , 1273-1373     , 0.20   , Paucirova1970 , 1.196E-01 , 2.649E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1273-1373     , 1.00   , Brockner1979 , 1.259E+00  , -2.800E-04 , 1268.15-1800 , 20.00  , Gheribi2014 , 6.862E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 0.10   , O'Brien1957\n\
KF            , 3  , 58.0967  , 1.0                  , 1131.20 , 0.50   , Johnson1958 , 1771.00 , 0.00   , Ruff1922       , 2.646E+00 , 6.515E-04 , 1154.2-1310.2 , 1.00   , Yaffe1956     , 1.068E-01 , 2.378E+04 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 1141.2-1327.6 , 2.00   , Janz1988     , 8.552E-01  , -2.540E-04 , 1129.15-1800 , 20.00  , Gheribi2014 , 7.060E+01 , 0.000E+00  , 0.000E+00  , 0.000E+00 , 18.00  , Beilmann2013\n\
LiF-NaF-KF    , 8  , 41.2909  , 0.465-0.115-0.42     , 735.00  , 2.00   , Rogers1982  , 1843.15 , 0.0    , Williams 2006b , 2.579E+00 , 6.240E-04 , 933-1163      , 1.00   , Cibulkova2006 , 0.000E+00 , 0.000E+00 , 2.130E-01  , -1.200E+03 , 1.350E+06 , 770-970       , 2.00   , Toerklep1980 , -3.500E-01 , 1.300E-03  , 773-973      , 3.50   , An2015      , 4.032E+01 , 4.388E-02  , 0.000E+00  , 0.000E+00 , 2.00   , Rogers1982\n"

# Initialize
tstDat = tempfile.NamedTemporaryFile(delete=False)
tstDat.write(tst_data)
tstDat.close()
d = Default_Data_Store()
d.load(tstDat.name)
tp = Thermophysical_Properties()
tp.initialize(d)

# Set up a composition
tp.setComposition(["LiF","NaF","KF"],[0.465,0.115,0.42])

## Temperatures/Enthalpy
print(748.77152178380845, tp.t_h(1000))
print(tp.t_h(tp.h_t(970)),970)

## Density
print(tp.rho(700))
print(tp.rho_h(tp.h_t(700)))

## Heat Capacity
print(tp.cp(700))
print(tp.cp_h(tp.h_t(700)))

## Viscosity
print(tp.mu(700))
print(tp.mu_h(tp.h_t(700)))

## Conductivity
print(tp.k(700))
print(tp.k_h(tp.h_t(970)),tp.k(970))
```
