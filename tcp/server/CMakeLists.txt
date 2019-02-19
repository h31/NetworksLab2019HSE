cmake_minimum_required(VERSION 3.0.0)
project(hw01-chat-server VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

include(CTest)
enable_testing()

add_executable(hw01-chat-server main.cpp include/server.h src/server.cpp ../message/message.h ../message/message.cpp ../utils/utils.h ../utils/utils.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)