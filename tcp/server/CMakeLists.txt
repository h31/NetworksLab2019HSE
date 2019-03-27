cmake_minimum_required(VERSION 3.12)
project(server)

set(CMAKE_CXX_STANDARD 14)

find_package (Threads)

include_directories(include)
add_executable(server
        src/Server.cpp
        src/main.cpp
        src/Client.cpp
        src/serverMessages/ServerMessage.cpp
        src/serverMessages/MessageMessage.cpp
        src/serverMessages/ErrorMessage.cpp
        src/serverMessages/GroupMessageMessage.cpp
        src/serverMessages/DisconnectMessage.cpp
        src/serverMessages/LoginSuccessMessage.cpp
        src/clientMessages/ClientMessage.cpp
        src/clientMessages/LoginMessage.cpp
        src/clientMessages/SingleMessageMessage.cpp
        src/clientMessages/BroadcastMessageMessage.cpp
        src/Acceptor.cpp
        src/Worker.cpp)

target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
