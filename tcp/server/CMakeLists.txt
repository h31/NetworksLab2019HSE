cmake_minimum_required(VERSION 3.0.0)
project(hw01-calc-server VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)
add_executable(hw01-calc-server main.cpp include/server.h src/server.cpp ../message/include/message.h ../message/message.cpp)
target_link_libraries(hw01-calc-server ${CMAKE_THREAD_LIBS_INIT})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
