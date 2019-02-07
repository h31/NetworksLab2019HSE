cmake_minimum_required(VERSION 3.13)
project(message)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(message src/message.h src/message.cpp)