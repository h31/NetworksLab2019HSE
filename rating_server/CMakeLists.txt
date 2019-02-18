# cmake_minimum_required(VERSION <specify CMake version here>)
project(rating_server)

set(CMAKE_CXX_STANDARD 14)

if (UNIX)
    SET(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
endif()


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

if(WIN32)
    set(PLATFORM_SPECIFIC_SOURCE_FILES private/socket/WindowsSocket.cpp public/socket/WindowsSocket.h)
elseif(UNIX)
    set(PLATFORM_SPECIFIC_SOURCE_FILES private/socket/LinuxSocket.cpp public/socket/LinuxSocket.h private/Client.cpp public/Client.h)
endif()

add_executable(rating_server private/main.cpp ${PLATFORM_SPECIFIC_SOURCE_FILES} public/Communicator.h private/Communicator.cpp private/ConsoleChatController.cpp public/ConsoleChatController.h public/Messages.h public/socket/MySocket.h private/Server.cpp public/Server.h)

if(WIN32)
    target_link_libraries(rating_server -lws2_32)
endif()