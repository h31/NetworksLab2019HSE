cmake_minimum_required(VERSION 3.5)
project(homework02)

set(CMAKE_CXX_STANDARD 11)
find_package(Threads REQUIRED)

include_directories(.)

add_executable(homework02
        src/CurrencyServerApplication.cpp
        src/Server.cpp include/Server.h
        src/Currency.cpp include/Currency.h)

target_link_libraries(homework02 Threads::Threads)
