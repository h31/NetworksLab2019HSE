cmake_minimum_required(VERSION 3.5)
project(NetworksLab2019HSE)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Iinclude -pedantic -Wextra -pthread")

include_directories(./include)

add_executable(NetworksLab2019HSE
        src/Client.cpp
        src/CurrencyClientApplication.cpp
        src/Currency.cpp
        )

add_executable(runTests
        include/catch.hpp
        test/ClientTest.cpp
        src/Client.cpp
        src/Currency.cpp
        )