#!/bin/bash

build_path="web/server/build"

if [ -d $build_path ]; then
    rm -rf $build_path
fi 
mkdir $build_path && cd $build_path 

# 编译
cmake ../ && make -j
