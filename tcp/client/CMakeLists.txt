cmake_minimum_required(VERSION 3.10)
project(client)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(client src/main.cpp src/client.cpp src/socket_io.cpp include/client.h include/connection_opening_exception.h include/disconnected_server_exception.h include/no_such_host_exception.h include/socket_io.h include/nonexistend_product_exception.h include/socket_opening_exception.h include/socket_reading_exception.h include/socket_writing_exception.h)
