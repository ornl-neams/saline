################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################

#
# Incorporate GitLab CI Traceability 
# 
if(DEFINED ENV{CI_PROJECT_URL})
    message(STATUS "GitLab CI Job Info:  $ENV{CI_PROJECT_URL}/-/jobs/$ENV{CI_JOB_ID}")
endif()

IF(NOT DEFINED SALINE_DEBUG_OUTPUT)
   SET(SALINE_DEBUG_OUTPUT 0 CACHE STRING "Display debug output.")
ENDIF()
IF(SALINE_DEBUG_OUTPUT)
   ADD_DEFINITIONS("-DSALINE_DEBUG_OUTPUT=${SALINE_DEBUG_OUTPUT}")
ENDIF()

IF(NOT DEFINED SALINE_TIMING)
   SET(SALINE_TIMING 0 CACHE STRING "Timing setting.")
ENDIF()
IF(SALINE_TIMING)
   ADD_DEFINITIONS("-DSALINE_TIMING=${SALINE_TIMING}")
ENDIF()

IF(NOT DEFINED SALINE_DBC)
   SET(SALINE_DBC 0 CACHE STRING "DBC setting.")
ENDIF()
IF(SALINE_DBC)
   ADD_DEFINITIONS("-DSALINE_DBC=${SALINE_DBC}")
ENDIF()
IF(BUILD_SHARED_LIBS)
   ADD_DEFINITIONS("-DBUILD_SHARED")
ENDIF()

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (NOT TRIBITS_PROCESSING_PACKAGE)
  # This CMakeLists.txt file is being processed as the saline TriBITS projects's base
  # CMakeLists.txt file!  (See comments at bottom of this file.)
  CMAKE_MINIMUM_REQUIRED(VERSION 3.1 FATAL_ERROR)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/ProjectName.cmake")
  PROJECT(${PROJECT_NAME} NONE)
  # enable C++11 by default
  SET(saline_ENABLE_CXX11 ON CACHE BOOL "Compile using the C++11 standard" FORCE)
  # disable generating HTML dependencies webpage and xml files
  SET(${PROJECT_NAME}_DEPS_XML_OUTPUT_FILE OFF CACHE BOOL "")
  # disable TriBITS export system to save time configuring
  SET(${PROJECT_NAME}_ENABLE_INSTALL_CMAKE_CONFIG_FILES OFF CACHE BOOL "")
  SET(${PROJECT_NAME}_ENABLE_EXPORT_MAKEFILES OFF CACHE BOOL "")
  SET(${PROJECT_NAME}_TRIBITS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/TriBITS/tribits" CACHE PATH "")
  INCLUDE("${${PROJECT_NAME}_TRIBITS_DIR}/TriBITS.cmake")
  SET(${PROJECT_NAME}_ENABLE_TESTS ON CACHE BOOL "Enable tests by default.")
  # default fortran support off
  IF(NOT ${PROJECT_NAME}_ENABLE_Fortran)
     SET(${PROJECT_NAME}_ENABLE_Fortran OFF CACHE BOOL "Enable fortran support")
  ENDIF()
  set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -ffree-line-length-none")
  # default MPI support off
  IF(NOT TPL_ENABLE_MPI)
    SET(TPL_ENABLE_MPI OFF CACHE BOOL "Enable MPI support")
  ENDIF()
  #
  # For windows with BUILD_SHARED_LIBS we must use CMAKE_RUNTIME_OUTPUT_DIRECTORY
  # to place all *dll and *exe in the same directory so unit tests will work
  IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" AND BUILD_SHARED_LIBS)
    IF(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
      SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/cmake_runtime_output" CACHE STRING "")
    ENDIF()
  ENDIF()

  TRIBITS_PROJECT_ENABLE_ALL()
ELSE()
  # This CMakeLists.txt file is being processed as the TriBITS package file.
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
  # include binary directory for configured includes
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
  TRIBITS_PACKAGE_DECL(saline)
  #
  # Process subpackages
  #
  TRIBITS_PROCESS_SUBPACKAGES()
  TRIBITS_PACKAGE_DEF()
  TRIBITS_PACKAGE_POSTPROCESS()
ENDIF()

# NOTE: In order to allow the `saline/` directory to be both a TriBITS
# package (for inclusion in other TriBITS projects) and to be a TriBITS
# project itself, you only have to put in a simple if statement in this
# top-level CMakeLists.txt file.  That is all!
