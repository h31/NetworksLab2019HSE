cmake_minimum_required(VERSION 2.8)
project(server)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        src/main.cpp)

include_directories(include)

add_executable(server ${SOURCE_FILES})

