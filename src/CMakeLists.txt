################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("Configuring Saline library")


add_library(saline SHARED)
set_target_properties(saline PROPERTIES
  SOVERSION
    ${saline_VERSION_MAJOR}
  VERSION
    ${saline_VERSION}
)
# The Core Saline library
add_subdirectory(cpp)
target_sources(saline PRIVATE
  "$<TARGET_OBJECTS:saline-cpp>"
)

# The Fortran wrapper, this is linked into the main library when configured
if (NOT DEFINED saline_ENABLE_Fortran )
  set(saline_ENABLE_Fortran OFF)
endif()
if( saline_ENABLE_Fortran )
  add_subdirectory(fortran)
  target_sources(saline PRIVATE
    "$<TARGET_OBJECTS:ForSaline>"
)
endif()

# The Python wrapper, this is currently a separate library due to python conventions
if (NOT DEFINED saline_ENABLE_Python )
  set(saline_ENABLE_Python OFF)
endif()
if( saline_ENABLE_Python )
  add_subdirectory(python)
endif()

# Install the librarty
include(GNUInstallDirs)
set(saline_INSTALL_CMAKEDIR
  ${CMAKE_INSTALL_LIBDIR}/cmake/saline
  CACHE STRING "Path to saline cmake files"
)

configure_file(
  ${saline_SOURCE_DIR}/pkg/saline-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/saline-config.cmake @ONLY
)
configure_file(
   ${saline_SOURCE_DIR}/saline-config-version.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/saline-config-version.cmake @ONLY
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/saline-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/saline-config-version.cmake
  DESTINATION ${saline_INSTALL_CMAKEDIR}
)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/saline-${saline_VERSION})

install(TARGETS saline
  EXPORT
    saline_target
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT saline_Runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT saline_Runtime
    NAMELINK_COMPONENT saline_Development
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT saline_Development
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/saline-${saline_VERSION}
    COMPONENT saline_Development
)
install(EXPORT saline_target
  DESTINATION ${saline_INSTALL_CMAKEDIR}
  NAMESPACE ON::
  FILE saline-Targets.cmake
  COMPONENT saline_Development
)
