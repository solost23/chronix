#!/bin/bash

if [ -d "build" ]; then
    rm -rf build
fi 
mkdir build && cd build 

# 编译
cmake ../src/ && make -j
# 删除临时文件
cd .. && rm -rf build 

# 执行
cd ./bin && ./*
