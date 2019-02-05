cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(server src/main.cpp src/server.cpp src/server.h src/client_abstraction.cpp src/client_abstraction.h)