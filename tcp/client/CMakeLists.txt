cmake_minimum_required(VERSION 3.0.0)
project(hw01-chat-client VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(hw01-chat-client main.cpp include/client.h src/client.cpp ../message/message.h ../message/message.cpp ../utils/utils.h ../utils/utils.cpp)
