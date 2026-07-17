################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

#First pass automated version increment
project(saline
  VERSION 0.4.0
  DESCRIPTION "MSTDB-TP API"
  LANGUAGES CXX
)

# Supported standards
set(CMAKE_CXX_STANDARD 17) #c++20 is not fully supported on likely compilers
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Declare options
option(${PROJECT_NAME}_USE_EXTERNAL_JSON "Use an external JSON library" OFF)
option(${PROJECT_NAME}_ENABLE_tests "Build tests" ON)
option(${PROJECT_NAME}_ENABLE_hdf5 "Use hdf5" OFF)
option(${PROJECT_NAME}_ENABLE_Fortran "Build Fortran bindings" ON)
option(${PROJECT_NAME}_ENABLE_Python "Build Python bindings" ON)

# Configure jsons
add_subdirectory(thirdparty)
# Configure the library
add_subdirectory(src)

# Testing is enabled unless explicitly turned off
if (${PROJECT_NAME}_ENABLE_tests )
  include(CTest)
  add_subdirectory(tests)
endif()

# Incorporate GitLab CI Traceability 
if( DEFINED ENV{CI_PROJECT_URL} )
  message(STATUS "GitLab CI Job Info:  $ENV{CI_PROJECT_URL}/-/jobs/$ENV{CI_JOB_ID}")
endif()
