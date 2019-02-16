cmake_minimum_required(VERSION 3.5)
project(NetworksLab2019HSE)

# set(TBB_DIR lib/tbb)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Iinclude -pedantic -Wextra -pthread")

include_directories(./include)

# include(./cmake/TBBGet.cmake)
# tbb_get(TBB_ROOT tbb_root CONFIG_DIR TBB_DIR)
# find_package(TBB)

add_executable(NetworksLab2019HSE
        src/Client.cpp
        src/PrimesServerApplication.cpp
        src/Currency.cpp
        src/Server.cpp include/Server.h src/PrimeNumbers.cpp include/PrimeNumbers.h src/PrimeNumbersConcurrent.cpp include/PrimeNumbersConcurrent.h)

add_executable(runTests
        include/catch.hpp
        test/ClientTest.cpp
        src/Client.cpp
        src/Currency.cpp
        src/Server.cpp include/Server.h)

# target_link_libraries(NetworksLab2019HSE ${TBB_IMPORTED_TARGETS})