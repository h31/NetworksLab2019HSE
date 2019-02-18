cmake_minimum_required(VERSION 3.10)
project(rating_server)

set(CMAKE_CXX_STANDARD 14)

SET(GCC_COVERAGE_COMPILE_FLAGS "-pthread")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")


include_directories(public)

add_executable(rating_server
        private/main.cpp
        private/ConsoleChatController.cpp
        public/ConsoleChatController.h
        public/ServerMessage.h
        private/Server.cpp
        public/Server.h
        public/ClientMessages/BaseMessage.h
        private/ClientMessages/Connect.cpp
        public/ClientMessages/Connect.h
        private/ClientMessages/NewRating.cpp
        public/ClientMessages/NewRating.h
        private/Socket.cpp
        public/Socket.h
        private/Client.cpp
        public/Client.h public/ClientMessages/ClientMessage.h private/ClientMessages/DeleteRating.cpp public/ClientMessages/DeleteRating.h private/ClientMessages/OpenRating.cpp public/ClientMessages/OpenRating.h private/ClientMessages/CloseRating.cpp public/ClientMessages/CloseRating.h private/ClientMessages/AddChoice.cpp public/ClientMessages/AddChoice.h private/ClientMessages/RatingList.cpp public/ClientMessages/RatingList.h private/ClientMessages/ShowRating.cpp public/ClientMessages/ShowRating.h private/ClientMessages/VoteRating.cpp public/ClientMessages/VoteRating.h public/Rating.cpp public/Rating.h)
