#!/bin/bash

if [ ! -e build ];
then
  mkdir -p build
  cd build
  echo "Install prefix: $INSTALL_PREFIX"
  cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX ..
  cd ..
fi
