# Thermophysical Properties
This project provides an interface for thermophysical properties used in molten salt systems.

All properties are evualted as saturated liquid.

 The default data is transcribed from 
 
 Jerden, James. Molten Salt Thermophysical Properties Database Development: 2019 Update. 
 United States: N. p., 2019. Web. doi:10.2172/1559846.
 
 Units: 
  - Conductivity  - Watts per Meter-Kelvin (W/m K)
  - Pressure      - Kilopascal (kPa)
  - Temperature   - Kelvin (K)
  - Enthalpy      - Joule per Kilogram (J/kg)
  - Viscosity     - Centipoise (cP) or milli Newton-second per square Meter (mN.s/m^2)
  - Specific Heat - Joules per Kelvin Mole (J/K mole)
  - Density       - Grams per Cubic Centimeter (g/cc)

## Design
The [`Thermophysical_Properties`](./src/thermophysical_properties.hh) class wraps a [`Data_Store`](./src/data_store.hh) instance.

The `Data_Store` interface provides an integration point for data and the data format and type (e.g.,CSV vs HDF5, functional vs tabular).

The Data_Store has a convience class `View` to insulate Data_Store access.


>Note:: Pressure parameters are ignored, currently.

>Note:: Properties as a function of enthalpy are not yet implemented and will return `NaN`

>Note:: Current data integrated is incorporated via the [`Default_Data_Store`](./src/default_data_store.hh)

## Test

Using any C++ 11 compliant compiler you can perform the following:

```bash
g++ tstdefault_data_store.cc -std=c++11 -g -O0 ../*.{cc,hh} -o tstdefault_data_store
./tstdefault_data_store
```

