cmake_minimum_required(VERSION 3.13)
project(BugTrackingServer)
find_package (Threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(server src/main.cpp
        src/BugTrackingServer.cpp include/BugTrackingServer.h
        src/UserService.cpp include/UserService.h
        include/Bug.h
)
target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})