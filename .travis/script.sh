#!/bin/bash

# Build the system and run the tests
#
mkdir -p build
cd build
cmake -DACE_BUILD_TESTS=ON ..
make -j 4
make test

# Echo the test logs
#
echo "[== CTest log ==]"
cat Testing/Temporary/LastTest.log

echo "[== libace log ==]"
cat tests/libace/tests.log

echo "[== codegen log ==]"
cat tests/codegen/tests.log
