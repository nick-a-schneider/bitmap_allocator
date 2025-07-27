# Define the library target
add_library(bitmap_allocator STATIC
    allocator.c
)

target_include_directories(bitmap_allocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>  # for installation if needed
)

target_compile_features(bitmap_allocator PUBLIC cxx_std_11)
