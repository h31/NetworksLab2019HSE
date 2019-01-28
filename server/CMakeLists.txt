project(server)

add_executable(${PROJECT_NAME} src/main.cpp src/Server.cpp src/EmailWithInfo.cpp)

target_link_libraries(${PROJECT_NAME}
    common
    ${TBB_IMPORTED_TARGETS}
)