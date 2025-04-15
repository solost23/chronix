#!/bin/bash

if [ -d "build" ]; then
    rm -rf build
fi 
mkdir build && cd build 

cmake ../src/ && make -j && $1

cd .. 
