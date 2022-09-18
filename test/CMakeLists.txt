add_executable(
    crow_test
    test_window.cpp
)

target_include_directories(crow_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(crow_test PRIVATE crow_window)

add_test(NAME crow_test COMMAND crow_test)
