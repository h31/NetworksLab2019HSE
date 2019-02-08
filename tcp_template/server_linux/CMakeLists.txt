cmake_minimum_required(VERSION 3.13)
project(server_linux)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(server_linux main.cpp ../../tcp/message/src/message.h ../../tcp/message/src/message.cpp)
