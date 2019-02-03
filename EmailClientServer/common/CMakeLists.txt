project(EmailCommon)

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

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

add_executable(EmailCommonTest test/main.cpp)
target_link_libraries(EmailCommonTest ${PROJECT_NAME})