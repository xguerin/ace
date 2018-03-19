#!/bin/bash

NUMPROC=4
TEMPDIR=$HOME/packages

set -e

function install_cmake {
  echo "Installing CMake 3.9.3"
  if [[ ! -e cmake-3.9.3 ]]; then
    wget http://www.cmake.org/files/v3.9/cmake-3.9.3.tar.gz
    tar -xzf cmake-3.9.3.tar.gz
    cd cmake-3.9.3
    ./configure
    make -j ${NUMPROC}
    sudo make install
  else
    cd cmake-3.9.3
    sudo make install > /dev/null
  fi
  cd ..
}

function install_re2 {
  echo "Installing RE2"
  if [[ ! -e re2 ]]; then
    git clone https://code.googlesource.com/re2
    cd re2
    make -j ${NUMPROC}
    sudo make install
  else
    cd re2
    sudo make install
  fi
  cd ..
}

function install_lemon {
  echo "Installing Lemon"
  if [[ ! -e lemon ]]; then
    mkdir lemon
    cd lemon
    curl "http://www.sqlite.org/src/raw/tool/lemon.c?name=e6056373044d55296d21f81467dba7632bbb81dc49af072b3f0e76338771497e" > lemon.c
    curl "http://www.sqlite.org/src/raw/tool/lempar.c?name=105d0d9cbe5a25d24d4769241ffbfc63ac7c09e6ccee0dc43dcc8a4c4ae4e426" > lempar.c
    clang -o lemon lemon.c
    sudo cp lemon /usr/local/bin
    sudo mkdir -p /usr/local/share/lemon
    sudo cp lempar.c /usr/local/share/lemon
  else
    cd lemon
    sudo cp lemon /usr/local/bin
    sudo mkdir -p /usr/local/share/lemon
    sudo cp lempar.c /usr/local/share/lemon
  fi
  cd ..
}

function install_googletest {
  echo "Installing GoogleTest"
  if [[ ! -e googletest ]]; then
    git clone https://github.com/google/googletest.git
    cd googletest
    cmake .
    make -j ${NUMPROC}
    sudo make install
  else
    cd googletest
    sudo make install
  fi
  cd ..
}

function install_yaml {
  echo "Installing YAML-CPP"
  [[ -e yaml-cpp ]] && rm -rf yaml-cpp
  git clone https://github.com/jbeder/yaml-cpp.git
  mkdir -p yaml-cpp/build
  cd yaml-cpp/build
  cmake -DBUILD_SHARED_LIBS=ON ..
  make -j ${NUMPROC}
  sudo make install
  cd ../..
}

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then

  # Get some packages from brew
  #
  brew install cmake lua python tclap re2 ragel jansson

  # Access the package directory
  #
  OLDDIR=$PWD
  mkdir -p ${TEMPDIR}
  cd ${TEMPDIR}

  # Install lemon
  #
  install_lemon

  # Install GoogleTest
  #
  install_googletest

  # Install YAML-CPP
  #
  install_yaml

  # Go back and remote TMPDIR
  #
  cd ${OLDDIR}

else

  # Get some packages from apt
  #
  sudo apt-get install liblua5.2-dev libtclap-dev libjansson-dev

  # Access the package directory
  #
  OLDDIR=$PWD
  mkdir -p ${TEMPDIR}
  cd ${TEMPDIR}

  # Install CMake 3.9.3
  #
  install_cmake

  # Install RE2
  #
  install_re2

  # Install GoogleTest
  #
  install_googletest

  # Install YAML-CPP
  #
  install_yaml

  # Go back and remote TMPDIR
  #
  cd ${OLDDIR}

fi
