cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads)
add_executable(client main.cpp ../message/message.cpp)
target_link_libraries (client ${CMAKE_THREAD_LIBS_INIT})