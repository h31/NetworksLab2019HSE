cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp src/login.cpp src/io_util.cpp src/main_cycle.cpp)

include_directories(../server/include include)

add_executable(client ${SOURCE_FILES})
