################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(saline
  VERSION 0.0.1
  DESCRIPTION "MSTDB-TP API"
  LANGUAGES CXX
)

# Supported standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure the library
add_subdirectory(src)

# Testing is enabled unless explicitly turned off
if (NOT DEFINED saline_ENABLE_tests )
  set(saline_ENABLE_tests ON)
endif()
if (saline_ENABLE_tests )
  include(CTest)
  add_subdirectory(tests)
endif()

# Incorporate GitLab CI Traceability 
if(DEFINED ENV{CI_PROJECT_URL})
    message(STATUS "GitLab CI Job Info:  $ENV{CI_PROJECT_URL}/-/jobs/$ENV{CI_JOB_ID}")
endif()
