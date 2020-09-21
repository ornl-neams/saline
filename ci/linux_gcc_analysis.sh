#!/bin/bash -ex
mkdir build
cd build
module load toolchain/gcc/4.8.3 cmake valgrind
export saline_ENABLE_Fortran=ON # Enable the Fortran library and bindings
cmake -MEMCHECK_COMMAND="$(which valgrind)" \
      -DBUILDNAME="$(uname -s)-GCC-4.8.3-Debug-${CI_BUILD_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=DEBUG \
      -Dsaline_ENABLE_COVERAGE_TESTING=ON \
      -DCOVERAGE_EXTRA_FLAGS="-s ${CI_PROJECT_DIR}/googletest -d" \
      -Dsaline_ENABLE_TESTS=ON \
      -Dsaline_ENABLE_ALL_PACKAGES=ON \
      ..

ctest --output-on-failure \
      -D Experimental -j 8 \
      -D ExperimentalMemCheck \
      -D ExperimentalCodeCoverage \
      -D ExperimentalSubmit
