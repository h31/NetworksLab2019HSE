cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(client src/main.cpp src/client.cpp include/client.h)
target_link_libraries(client LINK_PUBLIC message)