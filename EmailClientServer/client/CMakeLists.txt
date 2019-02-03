project(EmailClient)

add_executable(${PROJECT_NAME} src/main.cpp src/Client.cpp)
target_link_libraries(${PROJECT_NAME} EmailCommon)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)