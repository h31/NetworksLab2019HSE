cmake_minimum_required(VERSION 3.9)
project(networks)

set(TBB_DIR lib/tbb)
set(CMAKE_CXX_STANDARD 11)

include(cmake/TBBGet.cmake)
tbb_get(TBB_ROOT tbb_root CONFIG_DIR TBB_DIR)
find_package(TBB)

add_subdirectory(server/)
add_subdirectory(client/)
add_subdirectory(common/)