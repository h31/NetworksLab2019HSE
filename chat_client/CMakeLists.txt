 cmake_minimum_required(VERSION 3.10)
project(chat_client)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)



SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

if(WIN32)
    set(PLATFORM_SPECIFIC_SOURCE_FILES private/socket/WindowsSocket.cpp public/socket/WindowsSocket.h)
elseif(UNIX)
    set(PLATFORM_SPECIFIC_SOURCE_FILES private/socket/LinuxSocket.cpp public/socket/LinuxSocket.h)
endif()

add_executable(chat_client private/main.cpp ${PLATFORM_SPECIFIC_SOURCE_FILES} public/Communicator.h private/Communicator.cpp private/ConsoleChatController.cpp public/ConsoleChatController.h public/Messages.h public/socket/MySocket.h)

if(WIN32)
target_link_libraries(chat_client -lws2_32)
endif()

target_link_libraries(chat_client Threads::Threads)
