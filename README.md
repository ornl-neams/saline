# Thermophysical Properties
This project provides an interface for thermophysical properties used in molten salt systems.

All properties are evualted as saturated liquid.

 Data derivation is described in: 
 
 Henderson, Shane, Agca, Can, McMurray, Jake W., and Lefebvre, Robert Alexander. Saline: An API for Thermophysical Properties.
 United States: N. p., 2021.
 
 Units: 
  - Conductivity  - Watts per Meter-Kelvin (W/m K)
  - Pressure      - Kilopascal (kPa)
  - Temperature   - Kelvin (K)
*   Enthalpy      - Joule per mole (J/mole)
  - Viscosity     - Centipoise (cP) or milli Newton-second per square Meter (mN.s/m^2)
  - Specific Heat - Joules per Kelvin Mole (J/K mole)
  - Density       - Grams per Cubic Centimeter (g/cc)

## Design
The [`Thermophysical_Properties`](./src/thermophysical_properties.hh) class wraps a [`Data_Store`](./src/data_store.hh) instance.

The `Data_Store` interface provides an integration point for data and the data format and type (e.g.,CSV vs HDF5, functional vs tabular).

The Data_Store has a convience class `View` to insulate Data_Store access.


>Note:: Pressure parameters are ignored, currently.


>Note:: Current data integrated is incorporated via the [`Default_Data_Store`](./src/default_data_store.hh)

# Getting Started 
For individuals wanting to compile the code from source, below are the tested requirements and configurations.

## Requirements
    * C/C++ compiler C++11 compliant/capable
      * GCC-6.4 tested on Mac OS
    * Git 2.15+
    * CMake 3.1 or newer

## Configuration and Compilation

* Save the ssh-key in [code-int.ornl.gov](https://code-int.ornl.gov/profile/keys).
* Clone saline `git clone git@code-int.ornl.gov:lefebvre/saline.git ~/saline`
* Change directory into saline `cd ~/saline`
* Clone TriBITS `git clone https://github.com/lefebvre/TriBITS.git TriBITS` [TriBITS documentation](https://tribits.org/doc/TribitsDevelopersGuide.html)
* Clone extra repos `./TriBITS/tribits/ci_support/clone_extra_repos.py`
* Create a build directory `mkdir -p ~/build/saline`
* Change into the build `cd ~/build/saline`
* Create a configuration script in ~/build/. Let's call it `../configure.sh` (linux)

```
#!/bin/bash
# Linux bash file example
rm -rf CMake*
cmake \
 -D CMAKE_BUILD_TYPE:STRING=RELEASE \
 -D saline_ENABLE_ALL_PACKAGES:BOOL=ON \
 -D saline_ENABLE_TESTS:BOOL=ON \
 -D CMAKE_INSTALL_PREFIX=`pwd`/install \
 -G "Unix Makefiles" \
 ~/saline
```

Lastly, perform the following commands

```bash
../configure.sh    # execute the configuration script
make -j8 install   # compile and install the library and tests
ctest -V -j 8      # execute the tests (verbosely) 
```

### Enabling Fortran Support

To enable Fortran, export saline_ENABLE_Fortran=ON. 

The Fortran compiler must be 2003 compliant and support the ISO_C_BINDINGS.

```bash
export saline_ENABLE_Fortran=ON # Enable the Fortran library and bindings
../configure.sh    # execute the configuration script
make -j8 install   # compile and install the library and tests
ctest -V -j 8      # execute the tests (verbosely) 
```
