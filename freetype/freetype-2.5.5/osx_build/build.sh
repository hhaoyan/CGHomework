#!/bin/bash
set -e

mkdir build
cd build
../../configure
make -j4
cp .libs/libfreetype.a ../
cp .libs/libfreetype.dylib ../