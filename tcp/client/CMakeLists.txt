cmake_minimum_required(VERSION 2.8)
project(calculator)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES "src/*.cpp")

add_executable(calculator ${SOURCE_FILES} include/CalculatorApp.cpp include/CalculatorApp.hpp include/CalcuatorServerDriver.cpp include/CalcuatorServerDriver.hpp src/main.cpp include/requests.hpp include/ConcurrentQueue.hpp src/requests.cpp)

target_include_directories(calculator PRIVATE include)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(calculator PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(calculator "${CMAKE_THREAD_LIBS_INIT}")
endif()
