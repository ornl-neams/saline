################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("configuring python wrappers")

# Have to have these in order to generate the wrappers
find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(SWIG REQUIRED)

#Create some requisite files
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.in DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/README.md   DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/setup.cfg   DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${saline_SOURCE_DIR}/LICENSE            DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
configure_file(SalinePy.i SalinePy.i COPYONLY)

# Run swig manually vice using setuptools for better control of options
add_custom_command(
  COMMAND
    "${SWIG_EXECUTABLE}" -c++ -python -py3 -I${saline_SOURCE_DIR}/include SalinePy.i
  OUTPUT
    SalinePy_wrap.cxx
    SalinePy.py
  DEPENDS
    saline-cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/SalinePy.i"
    "${CMAKE_CURRENT_BINARY_DIR}/SalinePy.i"
    "${saline_SOURCE_DIR}/include/data_store.hh"
    "${saline_SOURCE_DIR}/include/default_data_store.hh"
    "${saline_SOURCE_DIR}/include/r_kister_data_store.hh"
    "${saline_SOURCE_DIR}/include/thermophysical_properties.hh"
  VERBATIM
)
# Generate the python interface
add_library(SalinePy INTERFACE)
add_dependencies(SalinePy saline-cpp)
target_sources(SalinePy PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/SalinePy_wrap.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/SalinePy.py
  )
target_include_directories(SalinePy INTERFACE ${Python_INCLUDE_DIRS})

#Creates setup.py using a version controlled skeleton with generator expressions
if(CMAKE_CONFIGURATION_TYPES)
  file(GENERATE OUTPUT setup$<$<NOT:$<CONFIG:Release>>:$<CONFIG>>.py INPUT ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)
else()
  file(GENERATE OUTPUT setup.py INPUT ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)
endif()

#Execute the setup file to generate a wheel
add_custom_target(SalinePy_wheel "${Python_EXECUTABLE}" -m build)
add_dependencies(SalinePy_wheel SalinePy)
add_custom_command(TARGET SalinePy_wheel POST_BUILD
  COMMAND
    ${Python_EXECUTABLE} -m pip install -e .
  )

# Include the dist/* files with the install
include(GNUInstallDirs)
install(
  DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/dist"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/python"
  FILES_MATCHING PATTERN "*.whl"
  )
