project(common)

add_library(${PROJECT_NAME}
        src/model/request.cpp
        src/model/model.cpp
        src/model/response.cpp
        src/serialization/primitive.cpp
        src/serialization/request.cpp
        src/serialization/response.cpp
        src/serialization/size_collector.cpp
        src/communication/communication.cpp
)
add_library(common::lib ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_executable(common-test test/main.cpp)
target_link_libraries(common-test common)