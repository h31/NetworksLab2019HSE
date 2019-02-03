cmake_minimum_required(VERSION 3.13)
project(networks-labs)

set(TBB_DIR lib/tbb)
set(CMAKE_CXX_STANDARD 17)

include(cmake/TBBGet.cmake)
tbb_get(TBB_ROOT tbb_root CONFIG_DIR TBB_DIR)
find_package(TBB)

add_subdirectory(EmailClientServer/client)
add_subdirectory(EmailClientServer/common)
add_subdirectory(EmailClientServer/server)
