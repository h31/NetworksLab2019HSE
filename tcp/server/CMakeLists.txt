cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lboost_system -lboost_date_time -lboost_thread")

find_package(Boost REQUIRED COMPONENTS thread)

add_executable(server
        src/main.cpp
        src/roulette_server.cpp src/roulette_server.h
        src/tcp_server.cpp src/tcp_server.h)
target_link_libraries(server LINK_PUBLIC message ${Boost_LIBRARIES})