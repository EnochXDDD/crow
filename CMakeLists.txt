cmake_minimum_required(VERSION 3.22.1)
project(crow VERSION 0.1.0)

set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "C++20 Required" FORCE)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++20 Standard" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "LLVM C++ Compiler" FORCE)

enable_testing()

if(ANDROID)
	find_library(ANDROID_LIBRARY android)
	find_library(ANDROID_LOG_LIBRARY log)
else()
    find_package(PkgConfig MODULE)
    find_package(glog REQUIRED)
    find_package(TBB REQUIRED)
    find_package(RapidJSON REQUIRED)
    find_package(X11)

    pkg_check_modules(XRANDR xrandr)
    pkg_check_modules(XCB xcb xcb-randr x11-xcb)
endif()

find_package(Vulkan MODULE)

add_subdirectory(src)
add_subdirectory(test)
