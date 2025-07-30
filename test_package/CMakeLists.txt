cmake_minimum_required(VERSION 3.15)
project(test_package C)

find_package(bitmap_allocator REQUIRED)

add_executable(test_bitmap_allocator main.c)
target_link_libraries(test_bitmap_allocator bitmap_allocator::bitmap_allocator)
