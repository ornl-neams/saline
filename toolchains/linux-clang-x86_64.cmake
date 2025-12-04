# toolchains/linux-clang-x86_64.cmake

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Compilers
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_Fortran_COMPILER gfortran)

# Optional: pin a specific version:
# set(CMAKE_C_COMPILER clang-18)
# set(CMAKE_CXX_COMPILER clang++-18)

# Optional: prefer LLVM binutils if available
# find_program(LLVM_AR NAMES llvm-ar)
# if(LLVM_AR)
#   set(CMAKE_AR "${LLVM_AR}")
# endif()
# find_program(LLVM_RANLIB NAMES llvm-ranlib)
# if(LLVM_RANLIB)
#   set(CMAKE_RANLIB "${LLVM_RANLIB}")
# endif()

