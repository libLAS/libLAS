#!/bin/bash
# Builds and tests libLAS
source ./bin/ci/common.sh
mkdir -p _build
cd _build
cmake ..
make -j ${NUMTHREADS}
make test
