cmake_minimum_required(VERSION 2.8)
project(lab1_server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

set(SOURCE_FILES main.cpp server.cpp server.h)
add_executable(lab1_server ${SOURCE_FILES})
