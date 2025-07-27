# Define the library target
add_library(mylib STATIC
    allocator.c
    allocator.h
)

target_include_directories(allocator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>  # for installation if needed
)

target_compile_features(allocator PUBLIC cxx_std_11)
