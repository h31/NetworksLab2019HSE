cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

add_executable(server src/main.cpp src/server.cpp src/server.h)