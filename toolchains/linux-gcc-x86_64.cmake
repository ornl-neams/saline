# toolchains/linux-gcc-x86_64.cmake

# Target system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Compilers
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_Fortran_COMPILER gfortran)

# Optional: if you have versioned GCC, pin them:
# set(CMAKE_C_COMPILER gcc-13)
# set(CMAKE_CXX_COMPILER g++-13)

# Optional: if you want to be explicit about binutils:
# find_program(GCC_AR NAMES gcc-ar)
# if(GCC_AR)
#   set(CMAKE_AR "${GCC_AR}")
# endif()
# find_program(GCC_RANLIB NAMES gcc-ranlib)
# if(GCC_RANLIB)
#   set(CMAKE_RANLIB "${GCC_RANLIB}")
# endif()

