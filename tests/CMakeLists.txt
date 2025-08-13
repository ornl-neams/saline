################################################################################
#                                                                              #
#             SALINE TriBITS Project and Package CMakeLists.txt File           #
#             Molten Salt Thermophysical Properties Interpolation              #
################################################################################
message("Configuring Saline libary tests")

# Declare what googletest to use
# TODO decide if this should only happen after running findGtest
find_package(GTest)
if(NOT ${GTest_FOUND})
  message("Warning: Required pacakge GTest not found! Attempting to build from source")
  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_GetProperties(googletest)
  if( NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    #There are simpler ways, this way allows for excluding Gtest from the install
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  include(GoogleTest)
endif()

# Project specific debug module
add_executable(tstsaline_bug tstsaline_bug.cc)
target_include_directories(tstsaline_bug PRIVATE ${PROJECT_SOURCE_DIR}/src/cpp)
target_link_libraries(tstsaline_bug PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json)
gtest_discover_tests(tstsaline_bug DISCOVERY_MODE PRE_TEST)

# C++ Library tests
add_executable(tstutils tstutils.cc)
target_link_libraries(tstutils PRIVATE saline-cpp GTest::gtest_main nlohmann_json::nlohmann_json)
gtest_discover_tests(tstutils DISCOVERY_MODE PRE_TEST)

add_executable(tstdefault_data_store tstdefault_data_store.cc)
target_link_libraries(tstdefault_data_store PRIVATE saline-cpp GTest::gtest_main nlohmann_json::nlohmann_json)
gtest_discover_tests(tstdefault_data_store DISCOVERY_MODE PRE_TEST)

add_executable(tstrk_data_store tstrk_data_store.cc)
target_link_libraries(tstrk_data_store PRIVATE saline-cpp GTest::gtest_main nlohmann_json::nlohmann_json)
gtest_discover_tests(tstrk_data_store DISCOVERY_MODE PRE_TEST)

if( ${PROJECT_NAME}_ENABLE_hdf5 )
    find_package(HDF5 REQUIRED COMPONENTS C)

    configure_file(tsth5.h5 ${CMAKE_CURRENT_BINARY_DIR}/tsth5.h5 COPYONLY)

    target_compile_definitions(tstdefault_data_store PRIVATE SALINE_USE_HDF5)
    target_include_directories(tstdefault_data_store PRIVATE "$<BUILD_INTERFACE:${HDF5_INCLUDE_DIRS}>" "$<INSTALL_INTERFACE:${HDF5_INCLUDE_DIRS}>")
    target_link_libraries(tstdefault_data_store PRIVATE ${HDF5_LIBRARIES})

    target_compile_definitions(tstrk_data_store PRIVATE SALINE_USE_HDF5)
    target_include_directories(tstrk_data_store PRIVATE "$<BUILD_INTERFACE:${HDF5_INCLUDE_DIRS}>" "$<INSTALL_INTERFACE:${HDF5_INCLUDE_DIRS}>")
    target_link_libraries(tstrk_data_store PRIVATE ${HDF5_LIBRARIES})

    target_compile_definitions(tstutils PRIVATE SALINE_USE_HDF5)
    target_include_directories(tstutils PRIVATE "$<BUILD_INTERFACE:${HDF5_INCLUDE_DIRS}>" "$<INSTALL_INTERFACE:${HDF5_INCLUDE_DIRS}>")
    target_link_libraries(tstutils PRIVATE ${HDF5_LIBRARIES})
endif()

if( saline_ENABLE_Python )
  find_package(Python COMPONENTS Interpreter)
  configure_file(tstSalinePython.py ${CMAKE_CURRENT_BINARY_DIR}/tstSalinePython.py COPYONLY)
  add_test(NAME tstSalinePython COMMAND ${Python_EXECUTABLE} -m unittest tstSalinePython.py)
endif()

# Fortran wrapper tests
if( saline_ENABLE_Fortran )
  #This is the only place in this library where Fortran is actually compiled
  enable_language(Fortran)
  add_executable(tstthermophysical_properties tstthermophysical_properties.f90)
  set_target_properties(tstthermophysical_properties PROPERTIES LINKER_LANGUAGE Fortran)
  add_test(NAME tstthermophysical_properties COMMAND tstthermophysical_properties)
  target_link_libraries(tstthermophysical_properties ForSaline saline-cpp nlohmann_json::nlohmann_json)
endif()
