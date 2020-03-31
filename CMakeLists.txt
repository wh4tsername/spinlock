cmake_minimum_required(VERSION 3.15)
project(spinlock)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(spinlock tests.cpp spinlock.cpp)