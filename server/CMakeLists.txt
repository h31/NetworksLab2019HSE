project(server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(server src/main.cpp ../common/src/Message.cpp ../common/include/Message.h src/ChatServer.cpp include/ChatServer.h src/Tasks.cpp include/Tasks.h ../common/include/constants.h include/utils.h)

target_link_libraries(${PROJECT_NAME} ${TBB_IMPORTED_TARGETS})
