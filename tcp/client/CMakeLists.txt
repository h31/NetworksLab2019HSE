cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)

add_executable(client src/main.cpp src/socket_io.cpp include/socket_io.h include/socket_reading_exception.h include/socket_writing_exception.h)