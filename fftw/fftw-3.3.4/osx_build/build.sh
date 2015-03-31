#!/bin/bash

set -e 
rm -rf build
mkdir build
cd build
../../configure

make -j4

cp .libs/libfftw3.a ../