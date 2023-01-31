add_library(
    crow_util STATIC
    u_json.cpp
)

target_link_libraries(crow_util PRIVATE ${RapidJSON_LIBRARY} glog::glog)
