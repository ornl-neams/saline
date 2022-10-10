################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("Configruing saline Fortran component")

# Configure object library, this just precludes copying DLLs around on Windows
add_library(ForSaline OBJECT)

# This target specification forgoes compiling the Fortran files until the client
# uses them. This saves clients from having to write interface blocks for the
# already defined interface.
target_sources(ForSaline
  PRIVATE
    data_storeInterface.cpp
    default_data_storeInterface.cpp
    r_kister_data_storeInterface.cpp
    thermophysical_propertiesInterface.cpp
  INTERFACE
    "$<INSTALL_INTERFACE:data_store_I.f90>"
    "$<INSTALL_INTERFACE:data_store_M.f90>"
    "$<INSTALL_INTERFACE:default_data_store_I.f90>"
    "$<INSTALL_INTERFACE:default_data_store_M.f90>"
    "$<INSTALL_INTERFACE:r_kister_data_store_I.f90>"
    "$<INSTALL_INTERFACE:r_kister_data_store_M.f90>"
    "$<INSTALL_INTERFACE:thermophysical_properties_I.f90>"
    "$<INSTALL_INTERFACE:thermophysical_properties_M.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/data_store_I.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/data_store_M.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/default_data_store_I.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/default_data_store_M.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/r_kister_data_store_I.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/r_kister_data_store_M.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thermophysical_properties_I.f90>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thermophysical_properties_M.f90>"
)
target_include_directories(ForSaline PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
  "$<INSTALL_INTERFACE:${INSTALL_PREFIX}/include/saline-${saline_VERSION}/fortran>"
)
set_target_properties(ForSaline
  PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE
)
target_link_libraries(ForSaline saline-cpp)

# Install the Fortran module file to limit the work required of users
include(GNUInstallDirs)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/saline-${saline_VERSION}/fortran"
  FILES_MATCHING PATTERN "*.f90"
  PATTERN "tests" EXCLUDE
  PATTERN "cix" EXCLUDE
)
