cmake_minimum_required(VERSION 3.13)
project(server)

IF (APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    set(CMAKE_CXX_FLAGS -I/usr/local/include)
ENDIF ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(Boost REQUIRED COMPONENTS thread)
find_package(Threads REQUIRED)

add_executable(server
        src/main.cpp
        src/roulette_server.cpp include/roulette_server.h
        src/tcp_server.cpp include/tcp_server.h)
target_link_libraries(server LINK_PUBLIC message ${Boost_LIBRARIES})