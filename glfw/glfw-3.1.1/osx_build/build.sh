#!/bin/bash

set -e
cmake ..
make
cp src/libglfw3.a .