cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client src/main.cpp src/client.cpp src/client.h)