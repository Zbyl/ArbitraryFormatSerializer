#!/usr/bin/env bash

#this script assumes, that:
#  - BOOST_ROOT variable is pointing to Boost root directory
#  - COMPILER variable is set to proper compiler

# quit on errors
set -e

echo "Running CMake with COMPILER=$COMPILER and BOOST_ROOT=$BOOST_ROOT"

export CXX="$COMPILER"
#export CC="clang-3.7"

mkdir build-zz
cd build-zz

cmake ../thrash
