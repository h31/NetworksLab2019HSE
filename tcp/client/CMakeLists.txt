cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)

add_executable(client
        main.cpp Client.cpp Client.h protocol.cpp protocol.h IncomingEvent.h)
