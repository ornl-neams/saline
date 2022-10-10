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
  - Enthalpy      - Joule per mole (J/mole)
  - Viscosity     - Centipoise (cP) or milli Newton-second per square Meter (mN.s/m^2)
  - Specific Heat - Joules per Kelvin Mole (J/K mole)
  - Density       - Grams per Cubic Centimeter (g/cc)

## Design
The [`Thermophysical_Properties`](./src/thermophysical_properties.hh) class wraps a [`Data_Store`](./src/data_store.hh) instance.

The `Data_Store` interface provides an integration point for data and the data format and type (e.g.,CSV vs HDF5, functional vs tabular).

The Data_Store has a convenience class `View` to insulate Data_Store access.


>Note:: Pressure parameters are ignored, currently.


>Note:: Current data integrated is incorporated via the [`Default_Data_Store`](./src/default_data_store.hh)

# Getting Started
For individuals wanting to compile the code from source, below are the tested requirements and configurations.

## Requirements
* C/C++ compiler C++11 compliant/capable
* CMake 3.16 or newer
* Git 2.15+ (Recommended)

## Configuration and Compilation
The Paths used below, `~/saline` and `~/build/saline`, are totally arbitrary. Feel free to chose paths that work for your workflow.

* Obtain local copy of source code
  * Via ssh (Requires [XCAMS](https://xcams.ornl.gov/xcams/) account)
    * [Setup ssh access](https://docs.gitlab.com/ee/user/ssh.html) (if not already completed)
      * Generate ssh key
      * Save ssh key to Gitlab profile [code.ornl.gov](https://code.ornl.gov/-/profile/keys)
    * Clone saline `git clone git@code.ornl.gov:NEAMS/saline.git ~/saline`
  * Via https:
    * Clone saline `https://code.ornl.gov/neams/saline.git ~/saline`
* Change directory into saline `cd ~/saline`
* Create a build directory `mkdir -p ~/build/saline`
* Change into the build directory `cd ~/build/saline`
* Configure the build:
  * Linux/Macos:
  ```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_BUILD_TYPE:STRING=RELEASE \
      -Dsaline_ENABLE_TESTS:BOOL=ON \
      ~/saline
    make
  ```
  * Windows
  ```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=BOOL=ON \
      ~\saline
    cmake --build .\ --target ALL_BUILD --config Release
  ```
  * Additional Saline specific configuration options will be covered as they are added.
* Run tests using `ctest`
  * A satisfactory build should yield test results like this (total number of tests may change at any time)
  ```
  100% tests passed, 0 tests failed out of 18

  Total Test time (real) =   0.46 sec
  ```
  >Note:: Testing is enabled by default and has a dependency on[googletest](https://github.com/google/googletest).
  Cmake resolves this dependency at configuration time using [FetchContent](https://cmake.org/cmake/help/v3.16/module/FetchContent.html).

  The option `-Dsaline_ENABLE_TESTS:BOOL=OFF` will disable testing if desired.

### Language Extension

Wrappers are generated for the [`Thermophysical_Properties`](./src/thermophysical_properties.hh) and the [`Default_Data_Store`](./src/default_data_store.hh) classes.

An experimental feature [`r_kister_data_store`](./src/r_kister_data_store.hh) is also wrapped, but casual users are discouraged from using this feature at this time.

#### Enabling Fortran Support

Fortran wrappers are currently generated during the development cycle. Unfortunately this process is not public at this time.

The Fortran compiler must be 2003 compliant and support the ISO_C_BINDINGS.

To enable Fortran, pass the option `-Dsaline_ENABLE_Fortran:BOOL=ON` as in:
```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_BUILD_TYPE:STRING=RELEASE \
      -Dsaline_ENABLE_TESTS:BOOL=ON \
      -Dsaline_ENABLE_Fortran:BOOL=ON \
      ~/saline
```

#### Enabling Python Support

Python Wrappers are generated using [SWIG-4.0.2](https://www.swig.org/download.html).

To enable Python, pass the option `-Dsaline_ENABLE_Python:BOOL=ON` as in:
```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_BUILD_TYPE:STRING=RELEASE \
      -Dsaline_ENABLE_TESTS:BOOL=ON \
      -Dsaline_ENABLE_Python:BOOL=ON \
      ~/saline
```

#### Additional Languages

Wrappers are added on an as needed basis. Thanks to [SWIG](https://www.swig.org) this process is relatively painless. Different languages have different requirements though, so mileage varies.

# Contributions

At this time the main development repository is protected. Users with [XCAMS](https://xcams.ornl.gov/xcams/) accounts are welcome to join this project and submit issues/branches to this repository for consideration.
Contributors with UCAMS accounts may be invited to join the development repository directly.
Users without UCAMS or [XCAMS](https://xcams.ornl.gov/xcams/) access should contact mstdb@ornl.gov for guidance on comment submission.
