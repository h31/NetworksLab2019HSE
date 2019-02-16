cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp include/response.hpp include/request.hpp)

include_directories(include)

add_executable(server ${SOURCE_FILES})
