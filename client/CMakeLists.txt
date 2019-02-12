project(client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(client src/main.cpp ../common/src/Message.cpp ../common/include/Message.h ../common/include/ctpl.h src/ChatClient.cpp include/ChatClient.h include/utils.h)