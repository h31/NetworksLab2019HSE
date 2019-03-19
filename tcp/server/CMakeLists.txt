cmake_minimum_required(VERSION 2.8)
project(server)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        src/main.cpp include/response.hpp src/server.cpp include/server.hpp src/wallet.cpp include/wallet.hpp include/logger.hpp)

include_directories(include)

find_package(Threads REQUIRED)

add_executable(server ${SOURCE_FILES})

target_link_libraries(server Threads::Threads)
