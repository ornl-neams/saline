include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


configure_file(tstSalinePython.py ${CMAKE_CURRENT_BINARY_DIR}/tstSalinePython.py COPYONLY)

ADD_CUSTOM_COMMAND(OUTPUT tstSalinePython.py
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/tstSalinePython.py
    DEPENDS saline)

ADD_CUSTOM_TARGET(tstSalinePython ALL
    DEPENDS SalinePy)

TRIBITS_ADD_TEST(
    ${PYTHON_EXECUTABLE}
    NOEXEPREFIX
    NOEXESUFFIX
    NUM_MPI_PROCS 1
    NAME tstSalinePython
    ARGS "tstSalinePython.py"
    ${ARGN}
)
