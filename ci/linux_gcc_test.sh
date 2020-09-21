#!/bin/bash -ex
mkdir build
cd build

module load toolchain/gcc/4.8.3 cmake-3.13.2

cmake -DBUILDNAME="$(uname -s)-Release-${CI_BUILD_REF_NAME}" \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -Dsaline_ENABLE_TESTS=ON \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -Dsaline_ENABLE_ALL_PACKAGES=ON \
      ..

ctest --output-on-failure \
      -D ExperimentalStart \
      -D ExperimentalBuild -j 8\
      -D ExperimentalSubmit \
      -D ExperimentalTest -j 8 \
      -D ExperimentalSubmit
