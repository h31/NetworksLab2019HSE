cmake_minimum_required(VERSION 3.13)
project(client)

IF (APPLE)
    # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
    set(CMAKE_CXX_FLAGS -I/usr/local/include)
ENDIF ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

find_package(Boost REQUIRED COMPONENTS thread)
find_package(Threads REQUIRED)

add_executable(client src/main.cpp src/client.cpp include/client.h)
target_link_libraries (client LINK_PUBLIC message ${Boost_LIBRARIES})