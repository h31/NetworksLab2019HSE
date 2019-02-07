cmake_minimum_required(VERSION 2.8)
project(client_linux)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

set(SOURCE_FILES main.c ../../tcp/message/src/message.cpp ../../tcp/message/message.h)
add_executable(client_linux ${SOURCE_FILES})