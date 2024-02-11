################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("Configuring Saline library")

add_library(${PROJECT_NAME} SHARED)
set(public_headers 
    ${saline_SOURCE_DIR}/include/utils.hh
    ${saline_SOURCE_DIR}/include/data_store.hh
    ${saline_SOURCE_DIR}/include/default_data_store.hh
    ${saline_SOURCE_DIR}/include/r_kister_data_store.hh
    ${saline_SOURCE_DIR}/include/thermophysical_properties.hh
    )
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE
  PUBLIC_HEADER
  "${public_headers}"
  SOVERSION
    ${saline_VERSION_MAJOR}
  VERSION
    ${saline_VERSION}
)
# The Core Saline library
add_subdirectory(cpp)
target_sources(${PROJECT_NAME} PRIVATE
  "$<TARGET_OBJECTS:saline-cpp>"
)

# The Fortran wrapper, this is linked into the main library when configured
if (NOT DEFINED ${PROJECT_NAME}_ENABLE_Fortran )
  set(${PROJECT_NAME}_ENABLE_Fortran OFF)
endif()
if( ${PROJECT_NAME}_ENABLE_Fortran )
  add_subdirectory(fortran)
  target_sources(${PROJECT_NAME} PRIVATE "$<TARGET_OBJECTS:ForSaline>")
endif()

# The Python wrapper, this is currently a separate library due to python conventions
if (NOT DEFINED ${PROJECT_NAME}_ENABLE_Python )
  set(${PROJECT_NAME}_ENABLE_Python OFF)
endif()
if( ${PROJECT_NAME}_ENABLE_Python )
  add_subdirectory(python)
endif()

# Install the library
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )
install(EXPORT ${PROJECT_NAME}Targets
  FILE "${PROJECT_NAME}Targets.cmake"
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION cmake
)

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)
# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)
# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)
