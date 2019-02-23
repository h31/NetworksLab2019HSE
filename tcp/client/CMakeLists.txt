cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(client
        include/Client.h src/Client.cpp
        include/ReadWriteHelper.h src/ReadWriteHelper.cpp
        src/Main.cpp
        CMakeLists.txt)
