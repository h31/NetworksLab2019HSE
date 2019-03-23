cmake_minimum_required(VERSION 3.13)
project(server)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS thread)

add_executable(server
        src/main.cpp
        src/market_server.cpp include/market_server.h
        src/tcp_server.cpp include/tcp_server.h)
target_link_libraries(server LINK_PUBLIC message ${Boost_LIBRARIES})