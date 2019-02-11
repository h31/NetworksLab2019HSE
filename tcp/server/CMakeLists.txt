cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pthread")

add_executable(server src/main.cpp src/server.cpp include/server.h)

target_link_libraries(${PROJECT_NAME} pthread)