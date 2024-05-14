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

## Basic Usage
```

    std::string file_path = "path/to/Molten_Salt_Thermophysical_Properties.csv";
    saline::Default_Data_Store ds = saline::Default_Data_Store();
    ds.load(file_path);
    if(ds.size() <= 0)
    {
        std::runtime_error("Failed to load data");
    }
    saline::Thermophysical_Properties tp = saline::Thermophysical_Properties();
    if(!tp.initialize(&ds))
    {
        std::runtime_error("Failed to initialize Thermophysical_Properties");
    }
    if(!tp.setComposition({"LiF","NaF","KF"},{0.465,0.115,0.42}))
    {
        std::runtime_error("Failed to load required salt");
    }
    double temperature = 900 // kelvin
    // Density demonstrated check thermophysical_properties.hh for other interfaces
    std::cout << tp.rho(900) << std::endl;

    return 0;
```

# Getting Started
For individuals wanting to compile the code from source, below are the tested requirements and configurations.

## Requirements
* C/C++ compiler C++11 compliant/capable
* CMake 3.23 or newer
* Git 2.15+ (Recommended)

## Configuration and Compilation
The Paths used below, `$HOME/saline` and `$HOME/saline/build`, are totally arbitrary. Feel free to chose paths that work for your workflow.

* Obtain local copy of source code
  * Decide on an appropriate source location, e.g. `export saline_src=$HOME/saline`
  * Decide on an appropriate build location, e.g. `export saline_bld=${saline_src}/build`
  * Via https:
    * Clone saline `https://code.ornl.gov/neams/saline.git ${saline_src}`
  * Via ssh (Requires a user account on code.ornl.gov)
    * [Setup ssh access](https://docs.gitlab.com/ee/user/ssh.html) (if not already completed)
      * Generate ssh key
      * Save ssh key to Gitlab profile [code.ornl.gov](https://code.ornl.gov/-/profile/keys)
    * Clone saline `git clone git@code.ornl.gov:NEAMS/saline.git ${saline_src}`
* Configure and build:
  * Linux/Macos:
  ```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_BUILD_TYPE:STRING=RELEASE \
      -Dsaline_ENABLE_TESTS:BOOL=ON \
      -S ${saline_src} \
      -B ${saline_bld}
    cmake --build ${saline_bld}
  ```
  * Windows
  ```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=BOOL=TRUE \
      -S ${saline_src} \
      -B ${saline_bld}
    cmake --build ${saline_bld} --target ALL_BUILD --config RELEASE
  ```
  * Additional Saline specific configuration options will be covered as they are added.
* Run tests using `ctest --test-dir ${saline_bld}`
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
      -S ${saline_src} \
      -B ${saline_bld}
```

#### Enabling Python Support

Python Wrappers are generated using [SWIG-4.2.1](https://www.swig.org/download.html).

In principal earlier versions of SWIG may be used with a few constraints. Python versions 3.9 and later require SWIG version greater than 4.1.0.

Additionally, the python packages 'build' and 'setuptools' must be installed. This may be done via pip or conda/mamba.

For convenience, user's can utilize conda and this [environment yaml](./ci/build_env.yml) to get started. Not all packages listed therein are required simply for building.

To enable Python, pass the option `-Dsaline_ENABLE_Python:BOOL=ON` as in:
```
    cmake \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -DCMAKE_BUILD_TYPE:STRING=RELEASE \
      -Dsaline_ENABLE_TESTS:BOOL=ON \
      -Dsaline_ENABLE_Python:BOOL=ON \
      -S ${saline_src} \
      -B ${saline_bld}
    cmake --build ${saline_bld}
```

This should result in having a python wheel file created at `${saline_bld}/src/python/dist/SalinePy*.whl`. It should be noted that this process only is only sufficient for sharing a wheel among local users of a given machine, or optimistically a substantially similar machine. Building a wheel for general consumption is beyond the scope and questions on the process should be directed to mstdb@ornl.gov.

See [Python package](./src/python/README.md) for a simple usage pattern.

#### Additional Languages

Wrappers are added on an as needed basis. Thanks to [SWIG](https://www.swig.org) this process is relatively painless. Different languages have different requirements though, so mileage varies.

# Contributions

At this time the main development repository is protected. External users should direct comments, questions, and suggestions to mstdb@ornl.gov.
