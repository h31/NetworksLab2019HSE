cmake_minimum_required(VERSION 3.7)
project(test_sys)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)
set(SOURCE_FILES main.cpp server.cpp server.h)
add_executable(test_sys ${SOURCE_FILES})