cmake_minimum_required(VERSION 3.5)
project(homework01)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(homework01
        src/PrimeNumbersClientApplication.cpp
        src/Client.cpp include/Client.h)

