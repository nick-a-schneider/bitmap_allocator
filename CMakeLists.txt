cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(bitmap_allocator C)

option(BUILD_TESTING "Build unit tests" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/generators ${CMAKE_PREFIX_PATH})

add_library(bitmap_allocator STATIC src/allocator.c)

target_include_directories(bitmap_allocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(bitmap_allocator::bitmap_allocator ALIAS bitmap_allocator)

install(TARGETS bitmap_allocator
    EXPORT bitmap_allocatorTargets
    ARCHIVE DESTINATION lib        # <-- Use 'lib', not 'bitmap_allocator'
    LIBRARY DESTINATION lib        # For shared libs if you add them later
    RUNTIME DESTINATION bin        # For executables if any
)

install(EXPORT bitmap_allocatorTargets
    FILE bitmap_allocatorTargets.cmake
    NAMESPACE bitmap_allocator::
    DESTINATION lib/cmake/bitmap_allocator
)

install(DIRECTORY include/ DESTINATION include)
