project(server)

add_executable(${PROJECT_NAME} src/main.cpp src/Server.cpp src/EmailWithInfo.cpp)

target_link_libraries(${PROJECT_NAME}
    common::lib
    ${TBB_IMPORTED_TARGETS}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)