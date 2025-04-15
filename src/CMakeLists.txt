# cmake_minimum_required(VERSION 3.10)
# project(DKR)

# # 设置 C++ 标准
# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # 设置编译器
# set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "g++")

# # 设置架构
# set(CMAKE_OSX_ARCHITECTURES arm64)

# # 添加包含路径
# include_directories(${CMAKE_SOURCE_DIR}/public/include)

# # 添加可执行文件
# add_executable(main ${CMAKE_SOURCE_DIR}/main.cpp)

cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME chronix)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

# set(CMAKE_OSX_ARCHITECTURES arm64)

execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Detected architecture: ${ARCH}")
set(CMAKE_OSX_ARCHITECTURES ${ARCH})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.cpp)
