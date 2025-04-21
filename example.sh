#!/bin/bash

if [ -d "example/build" ]; then
    rm -rf example/build
fi 
mkdir example/build && cd example/build 

# 编译
cmake ../ && make -j
# 删除临时文件
cd ../../ && rm -rf example/build 

# 执行
cd ./bin && ./example 
