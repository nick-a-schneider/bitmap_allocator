find_package(test_utils REQUIRED)

add_executable(test_allocator test_allocator.c)
target_link_libraries(test_allocator PRIVATE bitmap_allocator test_utils::test_utils)

add_test(NAME allocator_tests COMMAND test_allocator)
