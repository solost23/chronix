#!/bin/bash

if [ -d "performance/build" ]; then
    rm -rf performance/build
fi 
mkdir performance/build && cd performance/build 

# 编译
cmake ../ && make -j
# 删除临时文件
cd ../../ && rm -rf performance/build 

# 执行
cd ./bin && ./performance 
