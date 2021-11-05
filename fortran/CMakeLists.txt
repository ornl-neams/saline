TRIBITS_SUBPACKAGE(fortran)

SET(HEADERS
data_storeInterface.h
default_data_storeInterface.h
r_kister_data_storeInterface.h
thermophysical_propertiesInterface.h
)

SET(C_SOURCES
data_storeInterface.cpp
default_data_storeInterface.cpp
r_kister_data_storeInterface.cpp
thermophysical_propertiesInterface.cpp
)

SET(Fortran_SOURCES
data_store_I.f90
data_store_M.f90
default_data_store_I.f90
default_data_store_M.f90
r_kister_data_store_I.f90
r_kister_data_store_M.f90
thermophysical_properties_I.f90
thermophysical_properties_M.f90
)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
)

TRIBITS_ADD_LIBRARY(
  ${PACKAGE_NAME}_CInterface
  SOURCES ${C_SOURCES}
)


TRIBITS_ADD_LIBRARY(
  ${PACKAGE_NAME}_FInterface   
  SOURCES ${Fortran_SOURCES}
)

TARGET_LINK_LIBRARIES(${PACKAGE_NAME}_FInterface ${PACKAGE_NAME}_CInterface )

TRIBITS_ADD_TEST_DIRECTORIES( tests )

TRIBITS_SUBPACKAGE_POSTPROCESS()
