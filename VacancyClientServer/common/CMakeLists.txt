project(VacancyCommon)

add_library(${PROJECT_NAME} src/util.cpp src/model.cpp)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)