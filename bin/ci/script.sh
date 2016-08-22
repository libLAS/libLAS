#!/bin/bash
# Builds and tests libLAS
source ./bin/ci/common.sh
mkdir -p _build
cd _build

echo "$(tmstamp) *** script::cmake-config starting $(date) ***"
cmake .. -DWITH_GDAL=ON -DWITH_GEOTIFF=ON -DCMAKE_BUILD_TYPE=Debug
echo "$(tmstamp) *** script::cmake-config finished $(date) ***"

echo "$(tmstamp) *** script::cmake-build make -j ${NUMTHREADS} $(date) ***"
make -j ${NUMTHREADS}
echo "$(tmstamp) *** script::cmake-build finished $(date) ***"

echo "$(tmstamp) *** script::cmake-test starting $(date) ***"
LD_LIBRARY_PATH=./bin/Debug ./bin/Debug/liblas_test ../test/data
echo "$(tmstamp) *** script::cmake-test finished $(date) ***"
