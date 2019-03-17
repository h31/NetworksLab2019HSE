cmake_minimum_required(VERSION 3.9)
project(vacancy-server)

set(CMAKE_CXX_STANDARD 11)

set(TBB_DIR lib/tbb)
include(cmake/TBBGet.cmake)
tbb_get(TBB_ROOT tbb_root CONFIG_DIR TBB_DIR)
find_package(TBB)

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
    IF(MSVC)
        GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
        SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
        SET(Sources ${${SourcesVar}})

        SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                OBJECT_OUTPUTS "${PrecompiledBinary}")
        SET_SOURCE_FILES_PROPERTIES(${Sources}
                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                OBJECT_DEPENDS "${PrecompiledBinary}")
        # Add precompiled header to SourcesVar
        LIST(APPEND ${SourcesVar} ${PrecompiledSource})
    ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

ADD_MSVC_PRECOMPILED_HEADER(${PROJECT_NAME} "pch.h" "src/pch.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}\
    -pthread\
    -Werror\
    -Wall\
    -Wextra\
    -Winit-self\
    -Wold-style-cast\
    -Woverloaded-virtual\
    -Winit-self\
    -pedantic-errors\
    -Wuninitialized\
    -Wuseless-cast\
    -Woverloaded-virtual\
    -Wnon-virtual-dtor\
    -Wconversion\
    -Wtrigraphs\
    -Wlogical-op\
    -Wfloat-equal\
    -Wstrict-overflow=4\
    -fsanitize=leak\
    -fsanitize=undefined\
    -fomit-frame-pointer\
")

add_executable(
        ${PROJECT_NAME}
        src/main.cpp

        src/server.cpp
        src/vacancy.cpp
        src/speciality.cpp

        src/network/socket_exception.cpp
        src/network/server_socket.cpp
        src/network/client_socket.cpp

        src/util/user_view.cpp
)

# ubuntu 18
target_link_libraries(${PROJECT_NAME} TBB::tbb)

# ubuntu 16
#target_link_libraries(${PROJECT_NAME} ${TBB_IMPORTED_TARGETS})

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/lib
        PUBLIC ${PROJECT_SOURCE_DIR}/include

)