if(ANDROID)
    add_library(
        crow_window STATIC
        w_android.cpp
    )
else()
    add_library(
        crow_window STATIC
        w_xcb.cpp
    )
endif()

target_link_libraries(crow_window PRIVATE ${XCB_LIBRARIES} glog::glog)
