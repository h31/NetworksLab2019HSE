cmake_minimum_required(VERSION 3.13)
project(CalculatorServer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)
add_executable(CalculatorServer main.cpp server.h server.cpp)