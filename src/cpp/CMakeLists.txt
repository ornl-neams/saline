################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("Configuring Saline c++ component")

#TODO make these target definitions
## Set DBC
#IF(NOT DEFINED SALINE_DBC)
#   SET(SALINE_DBC 0 CACHE STRING "DBC setting.")
#ENDIF()
#IF(SALINE_DBC)
#   ADD_DEFINITIONS("-DSALINE_DBC=${SALINE_DBC}")
#ENDIF()

## Set debug output
#IF(NOT DEFINED SALINE_DEBUG_OUTPUT)
#   SET(SALINE_DEBUG_OUTPUT 0 CACHE STRING "Display debug output.")
#ENDIF()
#IF(SALINE_DEBUG_OUTPUT)
#   ADD_DEFINITIONS("-DSALINE_DEBUG_OUTPUT=${SALINE_DEBUG_OUTPUT}")
#ENDIF()

## Set timing
#IF(NOT DEFINED SALINE_TIMING)
#   SET(SALINE_TIMING 0 CACHE STRING "Timing setting.")
#ENDIF()
#IF(SALINE_TIMING)
#   ADD_DEFINITIONS("-DSALINE_TIMING=${SALINE_TIMING}")
#ENDIF()
#

# Configure object library, this just precludes copying DLLs around on Windows
add_library(saline-cpp OBJECT)
target_sources(saline-cpp
  PRIVATE
    utils.cc
    data_store.cc
    default_data_store.cc
    r_kister_data_store.cc
    thermophysical_properties.cc
)
target_include_directories(saline-cpp
  PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
  PUBLIC
    "$<BUILD_INTERFACE:${saline_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)
set_target_properties(saline-cpp PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# Handling symbol visibility
include(GenerateExportHeader)
generate_export_header(saline-cpp)
