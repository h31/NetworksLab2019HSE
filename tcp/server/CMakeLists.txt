cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/main.cpp include/PaymentSystemServer.h src/PaymentSystemServer.cpp
        include/ClientHandler.h src/ClientHandler.cpp
        include/Data.h src/Data.cpp
        include/ReadWriteHelper.h src/ReadWriteHelper.cpp
        )