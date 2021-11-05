##---------------------------------------------------------------------------##
## saline/CTestConfig.cmake
##---------------------------------------------------------------------------##

# Must match what is in CDash project 'saline'
SET(CTEST_NIGHTLY_START_TIME "02:00:00 UTC")
SET(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS 200)
set(CTEST_PROJECT_NAME "saline")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "ci.ornl.gov")
set(CTEST_DROP_LOCATION "/CDash/submit.php?project=saline")
set(CTEST_TRIGGER_SITE "")
set(CTEST_DROP_SITE_CDASH TRUE)

##---------------------------------------------------------------------------##
##                     end of saline/CTestConfig.cmake
##---------------------------------------------------------------------------##
