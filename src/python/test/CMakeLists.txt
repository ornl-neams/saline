################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("configuring SalinePy tests")

find_package(Python COMPONENTS Interpreter)
configure_file(tstSalinePython.py ${CMAKE_CURRENT_BINARY_DIR}/tstSalinePython.py COPYONLY)
add_test(NAME tstSalinePython 
  COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=$<TARGET_FILE_DIR:SalinePy>
  ${Python_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/tstSalinePython.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

