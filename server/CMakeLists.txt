add_executable(server src/main.cpp)
target_link_libraries(server ${TBB_IMPORTED_TARGETS})
target_link_libraries(server common)