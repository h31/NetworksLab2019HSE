cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp ../server/include/request.hpp src/login.cpp src/util.cpp src/main_cycle.cpp include/main_cycle.h)

include_directories(../server/include include)

add_executable(client ${SOURCE_FILES})