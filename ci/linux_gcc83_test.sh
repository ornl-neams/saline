#!/bin/bash

if [[ -f "/data/test_gcc/gcc-8.3.0/load_dev_env.sh" ]]; then BASE_DIR="/data/gitlab-runner"; . /data/test_gcc/gcc-8.3.0/load_dev_env.sh; ENV_BASE=/data/test_gcc/gcc-8.3.0; else BASE_DIR="/localhome/gitlab-runner"; . /projects/vera/gcc-8.3.0/load_dev_env.sh; ENV_BASE=/projects/vera/gcc-8.3.0; fi

cmake -DBUILDNAME="$(uname -s)-Release-${CI_BUILD_REF_NAME}" \
      -DCMAKE_C_COMPILER=${ENV_BASE}/toolset/gcc-8.3.0/bin/gcc \
      -DCMAKE_CXX_COMPILER=${ENV_BASE}/toolset/gcc-8.3.0/bin/g++ \
      -DCMAKE_BUILD_TYPE=RELEASE \
      -Dsaline_ENABLE_tests=ON \
      -DBUILD_SHARED_LIBS:BOOL=ON \
      -Dsaline_ENABLE_Fortran=ON \
      -DCMAKE_Fortran_FLAGS:STRING="-ffree-line-length-none" \
      -Dsaline_ENABLE_Python:BOOL=FALSE \
      $1

ctest --output-on-failure \
      -D ExperimentalStart \
      -D ExperimentalBuild -j 8\
      -D ExperimentalSubmit \
      -D ExperimentalTest -j 8 \
      -D ExperimentalSubmit
