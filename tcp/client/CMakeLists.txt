cmake_minimum_required(VERSION 2.8)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SOURCE_FILES src/main.cpp src/Client.cpp)
add_executable(client_linux ${SOURCE_FILES})