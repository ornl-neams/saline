##---------------------------------------------------------------------------##
## saline/Version.cmake
##---------------------------------------------------------------------------##
# Single file that needs to be changed on a release branch
# or on the development branch in order to configure saline
# for release mode and set the version.

SET(${PROJECT_NAME}_NAME "saline")
SET(${PROJECT_NAME}_VERSION 1.0.2)
SET(${PROJECT_NAME}_FULL_VERSION "beta 0.0.1")
SET(${PROJECT_NAME}_VERSION_STRING "0.0.1 (beta)")
SET(${PROJECT_NAME}_MAJOR_VERSION 01)
SET(${PROJECT_NAME}_MAJOR_MINOR_VERSION 000001)
SET(${PROJECT_NAME}_ENABLE_DEVELOPMENT_MODE_DEFAULT ON) # Change to 'OFF' for a release (beta or final)

##---------------------------------------------------------------------------##
##                    end of saline/Version.cmake
##---------------------------------------------------------------------------##
