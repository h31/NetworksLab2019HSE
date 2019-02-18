cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(client
        client.cpp
        client.h
        main.cpp)
