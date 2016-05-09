cmake_minimum_required(VERSION 3.4)
project(interlocked_functions)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -v")

set(SOURCE_FILES main.cpp Tests.cpp)
add_executable(interlocked_functions ${SOURCE_FILES})