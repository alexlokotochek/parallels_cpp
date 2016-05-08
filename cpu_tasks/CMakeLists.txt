cmake_minimum_required(VERSION 3.4)
project(cpu_tasks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp Futex.cpp Futex.h FutexVolatile.cpp FutexVolatile.h Tests.cpp Tests.h)
add_executable(cpu_tasks ${SOURCE_FILES})