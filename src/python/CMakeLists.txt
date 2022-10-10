################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("configuring python wrappers")

# Have to have these in order to generate the wrappers
find_package(Python REQUIRED)
find_package(SWIG REQUIRED)

# Run swig manually vice using setuptools for better control of options
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/SalinePy.i DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(SalinePy ALL 
  COMMAND
    "${SWIG_EXECUTABLE}" -c++ -python -py3 -I${saline_SOURCE_DIR}/include SalinePy.i
  BYPRODUCTS
    SalinePy_wrap.cxx
    SalinePy.py
)

#Creates setup.py using a version controlled skeleton with generator expressions
if(CMAKE_CONFIGURATION_TYPES)
  file(GENERATE OUTPUT setup$<$<NOT:$<CONFIG:Release>>:$<CONFIG>>.py INPUT ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)
else()
  file(GENERATE OUTPUT setup.py INPUT ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)
endif()

#Execute the setup file to generate a wheel
add_custom_target(SalinePy_wheel ALL "${Python_EXECUTABLE}" -m build)
add_dependencies(SalinePy_wheel saline-cpp)
