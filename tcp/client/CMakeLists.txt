cmake_minimum_required(VERSION 3.12)
project(client)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(client
        src/main.cpp
        src/Client.cpp
        src/Response.cpp
        src/request/RequestField.cpp
        src/SocketReader.cpp
        )
