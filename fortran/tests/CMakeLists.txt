file(GLOB tests *.f90)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")


foreach(test ${tests})
  get_filename_component(name ${test} NAME_WE)
  TRIBITS_ADD_EXECUTABLE_AND_TEST(${name} SOURCES ${test} LINKER_LANGUAGE Fortran COMM serial mpi NUM_MPI_PROCS 1)
ENDFOREACH()
