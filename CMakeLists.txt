cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(bitmap_allocator C)

option(BUILD_TESTS "Build unit tests" OFF)

add_library(bitmap_allocator STATIC src/allocator.c)

target_include_directories(bitmap_allocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(bitmap_allocator::bitmap_allocator ALIAS bitmap_allocator)

install(TARGETS bitmap_allocator
    EXPORT bitmap_allocatorTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(EXPORT bitmap_allocatorTargets
    FILE bitmap_allocatorTargets.cmake
    NAMESPACE bitmap_allocator::
    DESTINATION lib/cmake/bitmap_allocator
)

install(DIRECTORY include/ DESTINATION include)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()
