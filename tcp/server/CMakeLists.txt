cmake_minimum_required(VERSION 3.13)

set (BOOST_COMPONENTS thread)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_MULTITHREADED ON)

project(server)

set(CMAKE_CXX_STANDARD 17)

find_package (Threads)
find_package (Boost COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(server src/server.cpp include/server.h include/socket_opening_exception.h include/binding_exception.h src/socket_io.cpp include/socket_io.h include/socket_reading_exception.h include/socket_writing_exception.h include/product.h src/main.cpp)

target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})