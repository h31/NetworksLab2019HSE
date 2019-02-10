add_library (message src/message.cpp src/message.h)
target_include_directories (message PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
