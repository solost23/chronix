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
set(PROJECT_NAME server)
set(VERSION_NUMBER "1.0.0" CACHE STRING "Version")
set(CHRONIX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_BUILD_TYPE "Debug")

cmake_policy(SET CMP0048 NEW)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

add_compile_options(-Wall -fPIC)
message(STATUS "version: ${VERSION_NUMBER}")

# 设置可执行文件输出目录
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CHRONIX_ROOT_DIR}/bin)
#代码目录
set(SOURCE_CODE_DIR ${CHRONIX_ROOT_DIR}/src)

INCLUDE_DIRECTORIES(
    ${SOURCE_CODE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
)

file(GLOB_RECURSE SELF_PROJECT_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/routers/main.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/global/initialize/server.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/config/config.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/global/initialize/config.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/global/initialize/main.cpp 
)

execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "Detected architecture: ${ARCH}")
set(CMAKE_OSX_ARCHITECTURES ${ARCH})

ADD_EXECUTABLE(${PROJECT_NAME} ${SELF_PROJECT_SOURCE_FILES})

# 添加版本号宏定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    VERSION="${VERSION_NUMBER}"
)

if (${ARCH} STREQUAL "x86_64")
    set(MYSQLX_LIB_DIR "${CHRONIX_ROOT_DIR}/lib/mysqlx_x86_64")
    set(YAML_CPP_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/yaml-cpp_arm64")
elseif (${ARCH} STREQUAL "arm64" OR ${ARCH} STREQUAL "aarch64")
    set(MYSQLX_LIB_DIR "${CHRONIX_ROOT_DIR}/lib/mysqlx_arm64")
    set(YAML_CPP_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/yaml-cpp_x86_64")
else()
    message(FATAL_ERROR "Unsupported architecture: ${ARCH}")
endif()

find_library(mysqlcppconnxLIBRARY mysqlcppconnx HINTS ${MYSQLX_LIB_DIR})
if (mysqlcppconnxLIBRARY)
    message(STATUS "Found mysqlcppconnx library: ${mysqlcppconnxLIBRARY}")
    target_link_libraries(${PROJECT_NAME} ${mysqlcppconnxLIBRARY} ${SELF_INPUT_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find mysqlcppconnx library")
endif()

find_library(yaml-cppLIBRARY yaml-cpp HINTS ${YAML_CPP_LIB_DIR})
if (yaml-cppLIBRARY)
    message(STATUS "Found yaml-cpp library: ${yaml-cppLIBRARY}")
    target_link_libraries(${PROJECT_NAME} ${yaml-cppLIBRARY} ${SELF_INPUT_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find yaml-cpp library")
endif()

# # tests
# enable_testing()
# set (TEST_PROJECT_NAME test_chronix)

# add_executable(${TEST_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_chronix.cpp)

# find_library(mysqlcppconnxLIBRARY mysqlcppconnx HINTS ${MYSQLX_LIB_DIR})
# if (mysqlcppconnxLIBRARY)
#     message(STATUS "Found mysqlcppconnx library: ${mysqlcppconnxLIBRARY}")
#     target_link_libraries(${TEST_PROJECT_NAME} ${mysqlcppconnxLIBRARY} ${SELF_INPUT_LIBRARIES})
# else()
#     message(FATAL_ERROR "Could not find mysqlcppconnx library")
# endif()

# add_test(NAME ChronixTests COMMAND ${TEST_PROJECT_NAME})

# 下面可设置安装相关
