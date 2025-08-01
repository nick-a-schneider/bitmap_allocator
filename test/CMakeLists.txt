if(NOT TARGET test_utils)
    include(FetchContent)
    message(STATUS "Fetching test_utils...")
    FetchContent_Declare(
        test_utils
        GIT_REPOSITORY https://github.com/nick-a-schneider/test_utils.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(test_utils)
    add_library(test_utils::test_utils ALIAS test_utils)
endif()
if(NOT TARGET test_utils::test_utils)
    message(FATAL_ERROR "test_utils not found")
endif()

add_executable(test_allocator test_block_allocator.c)
target_link_libraries(test_allocator PRIVATE
    bitmap_allocator::bitmap_allocator 
    test_utils::test_utils
)

add_test(NAME allocator_tests COMMAND test_allocator)
