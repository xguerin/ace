#!/bin/bash

NUMPROC=4
TEMPDIR=$(mktemp -d ace.XXXXXX)

set -e

function install_lemon {
  echo "Installing Lemon"
  mkdir lemon
  cd lemon
  curl -s "http://www.sqlite.org/src/raw/tool/lemon.c?name=e6056373044d55296d21f81467dba7632bbb81dc49af072b3f0e76338771497e" > lemon.c
  curl -s "http://www.sqlite.org/src/raw/tool/lempar.c?name=105d0d9cbe5a25d24d4769241ffbfc63ac7c09e6ccee0dc43dcc8a4c4ae4e426" > lempar.c
  gcc -o lemon lemon.c
  cp lemon /usr/local/bin
  mkdir -p /usr/local/share/lemon
  cp lempar.c /usr/local/share/lemon
  cd ..
}

function install_googletest {
  echo "Installing GoogleTest"
  git clone https://github.com/google/googletest.git
  cd googletest
  cmake .
  make -j ${NUMPROC}
  make install
  cd ..
}

# Get some packages from apt
#
apt-get update
apt-get install -y cmake liblua5.2-dev libtclap-dev libjansson-dev ragel python-dev libre2-dev libyaml-cpp-dev curl git

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

# Go back and remove TEMPDIR
#
cd ${OLDDIR}
rm -rf ${TEMPDIR}
