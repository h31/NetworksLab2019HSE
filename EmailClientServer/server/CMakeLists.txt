project(EmailServer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(${PROJECT_NAME} src/main.cpp src/Server.cpp src/EmailWithInfo.cpp)

target_link_libraries(${PROJECT_NAME} EmailCommon ${TBB_IMPORTED_TARGETS})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)