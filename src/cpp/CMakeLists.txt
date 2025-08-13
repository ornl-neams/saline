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
      "${CMAKE_CURRENT_SOURCE_DIR}/utils.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/data_store.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/default_data_store.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/r_kister_data_store.cc"
      "${CMAKE_CURRENT_SOURCE_DIR}/thermophysical_properties.cc"
)
target_include_directories(saline-cpp
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<BUILD_INTERFACE:${saline_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)
target_link_libraries(saline-cpp PRIVATE nlohmann_json::nlohmann_json)
set_target_properties(saline-cpp PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

if( ${PROJECT_NAME}_ENABLE_hdf5 )
    find_package(HDF5 REQUIRED COMPONENTS C)
    target_compile_definitions(saline-cpp PRIVATE SALINE_USE_HDF5)
    target_include_directories(saline-cpp
    PRIVATE
        "$<BUILD_INTERFACE:${HDF5_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:${HDF5_INCLUDE_DIRS}>"
    )
    target_link_libraries(saline-cpp PRIVATE ${HDF5_LIBRARIES})
endif()

# Handling symbol visibility
include(GenerateExportHeader)
generate_export_header(saline-cpp)
