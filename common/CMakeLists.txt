add_library(common STATIC model/request.cpp model/request.h model/model.cpp model/model.h model/response.cpp model/response.h serialization/primitive.h serialization/request.h serialization/request.cpp serialization/response.h serialization/response.cpp serialization/size_collector.h serialization/size_collector.cpp)
target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
