cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp include/response.hpp include/request.hpp src/server.cpp include/server.h include/util.hpp src/pools.cpp include/pools.h)

include_directories(include)

add_executable(server ${SOURCE_FILES})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(server PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(server "${CMAKE_THREAD_LIBS_INIT}")
endif()