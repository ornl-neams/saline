# toolchains/windows-msvc-ifort.cmake

# We are targeting Windows
set(CMAKE_SYSTEM_NAME Windows)

# Use MSVC for C/C++
# CMake will find cl.exe from the environment (VS dev prompt / vcvarsall / oneAPI env)
set(CMAKE_C_COMPILER cl)
set(CMAKE_CXX_COMPILER cl)

# Fortran via Intel ifort
set(CMAKE_Fortran_COMPILER ifort)

# Optional but wise: tell CMake we are using MSVC-like ABI
set(CMAKE_C_COMPILER_ID MSVC)
set(CMAKE_CXX_COMPILER_ID MSVC)

# Don't touch build type, standards, etc. Those belong in project/presets.
