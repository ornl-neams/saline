TRIBITS_SUBPACKAGE(cpp)

TRIBITS_CONFIGURE_FILE(version.hh)

SET(SOURCE
data_store.cc
default_data_store.cc
thermophysical_properties.cc
utils.cc
)

SET(HEADERS
data_store.hh
default_data_store.hh
thermophysical_properties.hh
utils.hh
saline_bug.hh
)

#
# Add library
TRIBITS_ADD_LIBRARY(saline
  SOURCES ${SOURCE}
  NOINSTALLHEADERS ${HEADERS}
)

#
# Add test directory
TRIBITS_ADD_TEST_DIRECTORIES(tests)

INSTALL(FILES ${HEADERS} DESTINATION "include/")
TRIBITS_SUBPACKAGE_POSTPROCESS()
