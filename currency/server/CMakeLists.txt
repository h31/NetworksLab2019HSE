cmake_minimum_required(VERSION 3.5)
project(homework01)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads REQUIRED)

include_directories(.)

add_executable(homework01
        src/CurrencyServerApplication.cpp
        src/Server.cpp include/Server.h
        src/Currency.cpp include/Currency.h)

target_link_libraries(homework01 Threads::Threads)

