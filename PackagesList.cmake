##---------------------------------------------------------------------------##
## saline/PackagesList.cmake
##---------------------------------------------------------------------------##
#
# See documentation in TriBITS https://tribits.org


##---------------------------------------------------------------------------##
## PACKAGES PROVIDED
##---------------------------------------------------------------------------##

TRIBITS_REPOSITORY_DEFINE_PACKAGES(
  googletest googletest/googletest PT
  testframework testframework PT
  saline  . PT
)

TRIBITS_ALLOW_MISSING_EXTERNAL_PACKAGES(
   googletest
   testframework
)

##---------------------------------------------------------------------------##
##                     end of saline/PackagesList.cmake
##---------------------------------------------------------------------------##
