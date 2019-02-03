project(client)

add_executable(${PROJECT_NAME} src/main.cpp include/Client.h src/Client.cpp)
target_link_libraries(${PROJECT_NAME} common::lib)

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)