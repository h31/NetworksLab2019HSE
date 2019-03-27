cmake_minimum_required(VERSION 3.13)
project(Networks)
find_package (Threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(Networks main.cpp Task.cpp Task.h Server.cpp Server.h)
target_link_libraries (Networks ${CMAKE_THREAD_LIBS_INIT})