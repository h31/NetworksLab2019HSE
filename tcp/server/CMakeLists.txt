#[[
cmake_minimum_required(VERSION 3.13)
project(server C)

set(CMAKE_C_STANDARD 11)

add_executable(server main.c)

]]

####
#
#cmake_minimum_required(VERSION 3.8)
#project(msync_server)
#FIND_PACKAGE(PkgConfig)
#PKG_CHECK_MODULES(GLIB glib-2.0)
#include_directories(${GLIB_INCLUDE_DIRS})
#link_directories(${GLIB_LIBRARY_DIRS})
#add_definitions(${GLIB_CFLAGS_OTHER})
#set(CMAKE_C_STANDARD 11)
#set(SOURCE_FILES main.c)
#add_executable(server ${SOURCE_FILES} ${Glib_LIBRARY})

######

cmake_minimum_required(VERSION 3.1)
project(server)
#CFLAGS=
set(CMAKE_C_FLAGS "-L/opt/local/lib -I/opt/local/include")

include(FindPkgConfig)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

set(SOURCE_FILES main.c)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -iglib-2.0")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${GLIB_LIBRARIES})