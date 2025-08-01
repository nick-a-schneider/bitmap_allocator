#include "block_allocator.h"
#include "tier_allocator.h"
#include "test_utils.h"
#include <stdbool.h>

void test_InitTierAllocator(void) {
}

void test_Allocate(void) {
}

void test_DeblockAllocate(void) {
}

int main(void) {
    LOG_INFO("ALLOCATOR TESTS\n");
    TEST_EVAL(test_InitTierAllocator);
    TEST_EVAL(test_Allocate);
    TEST_EVAL(test_DeblockAllocate);
    return testGetStatus();
}