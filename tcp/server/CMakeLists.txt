cmake_minimum_required(VERSION 3.7)
project(test_sys)

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)
set(SOURCE_FILES main.cpp server.cpp server.h)
add_executable(test_sys ${SOURCE_FILES})
target_link_libraries(test_sys Threads::Threads)