cmake_minimum_required(VERSION 3.13)
project(client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lboost_system -lboost_date_time -lboost_thread")

find_package(Boost REQUIRED COMPONENTS thread)

add_executable(client src/main.cpp src/client.cpp src/client.h)
target_link_libraries (client LINK_PUBLIC message ${Boost_LIBRARIES})