cmake_minimum_required(VERSION 3.13)
project(market)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)

add_subdirectory (client)
add_subdirectory (message)
add_subdirectory (server)