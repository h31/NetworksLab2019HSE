cmake_minimum_required(VERSION 3.13)

project(server)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads)

add_executable(server src/server.cpp include/server.h include/socket_opening_exception.h include/binding_exception.h src/socket_io.cpp include/socket_io.h include/socket_reading_exception.h include/socket_writing_exception.h include/product.h src/main.cpp include/query_type.h)

target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})