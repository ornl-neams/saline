#!/bin/bash

export saline_ENABLE_Fortran=ON # Enable the Fortran library and bindings
cmake -DBUILDNAME="$(uname -s)-Release-${CI_BUILD_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -Dsaline_ENABLE_TESTS=ON \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -Dsaline_ENABLE_ALL_PACKAGES=ON \
      -Dsaline_ENABLE_salinepython:BOOL=FALSE \
      $1

ctest --output-on-failure \
      -D ExperimentalStart \
      -D ExperimentalBuild -j 8\
      -D ExperimentalSubmit \
      -D ExperimentalTest -j 8 \
      -D ExperimentalSubmit
