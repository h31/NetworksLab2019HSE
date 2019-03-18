cmake_minimum_required(VERSION 3.13)

add_library(message src/message.cpp include/message.h)
target_include_directories (message PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
