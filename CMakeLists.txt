################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

#First pass automated version increment
project(saline
  VERSION 0.2.0
  DESCRIPTION "MSTDB-TP API"
  LANGUAGES CXX
)

# Supported standards
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure the library
add_subdirectory(src)

# Testing is enabled unless explicitly turned off
if (NOT DEFINED ${PROJECT_NAME}_ENABLE_tests )
  set(${PROJECT_NAME}_ENABLE_tests ON)
endif()
if (${PROJECT_NAME}_ENABLE_tests )
  include(CTest)
  add_subdirectory(tests)
endif()

# Incorporate GitLab CI Traceability 
if(DEFINED ENV{CI_PROJECT_URL})
  message(STATUS "GitLab CI Job Info:  $ENV{CI_PROJECT_URL}/-/jobs/$ENV{CI_JOB_ID}")
endif()
