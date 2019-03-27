cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost REQUIRED COMPONENTS thread)

add_executable(client src/main.cpp src/client.cpp include/client.h)
target_link_libraries(client LINK_PUBLIC message)