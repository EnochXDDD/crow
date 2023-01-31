add_library(
    crow_graphics STATIC
    g_vulkan.cpp
)

target_link_libraries(crow_graphics PRIVATE Vulkan::Vulkan glog::glog TBB::tbb)
