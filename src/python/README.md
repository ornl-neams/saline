#SalinePy
This provides the python interface to the Saline library. See Saline for details on basic features.

These wrappers are still very much in development and likely do not represent the final product.

## Usage
Saline is striving to implement test driven design so the unit tests will be the best source of examples.

Nevertheless a basic use is provided below. NOTE: the data file must be supplied by user.


```
import tempfile
from SalinePy import Default_Data_Store, Thermophysical_Properties

## This should be the path to the mstdb-tp csv.
file_path = "my_path_to_the_data.csv" #TODO change for local conditions

# Initialize
d = Default_Data_Store()
d.load()
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
