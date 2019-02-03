cmake_minimum_required(VERSION 3.1)
project(NetworksLab2019HSE)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(NetworksLab2019HSE
        src/Client.cpp
        src/CurrencyClientApplication.cpp
        )
