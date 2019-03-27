cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/main.cpp src/server.cpp src/server.h
        src/ReadWriteHelper.h src/ReadWriteHelper.cpp src/ClientHandler.cpp src/ClientHandler.h
        )