cmake_minimum_required(VERSION 3.12)
project(networks-labs)

set(TBB_DIR lib/tbb)
set(CMAKE_CXX_STANDARD 17)

include(TBBGet.cmake)  # путь до TBBGet.cmake
tbb_get(TBB_ROOT tbb_root CONFIG_DIR TBB_DIR)
find_package(TBB)

add_subdirectory(CalculatorServer)