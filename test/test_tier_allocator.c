#include "block_allocator.h"
#include "tier_allocator.h"
#include "test_utils.h"
#include <stdbool.h>
#include <string.h>

#define MEMORY_SIZE 4096
uint8_t memory[MEMORY_SIZE];

void test_initTierAllocator(void) {
    TEST_CASE("initializing tier allocator") {
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        bool res = initTierAllocator(&allocator, (void*)memory, MEMORY_SIZE);
        ASSERT_TRUE(res, "initialization failed");

        uint16_t expected_size = MEMORY_SIZE / allocator.tiers;
        for (uint8_t i = 0; i < allocator.tiers; i++) {
            BlockAllocator* alloc = &allocator.allocators[i];
            ASSERT_EQUAL_INT(alloc->block_size, allocator.block_sizes[i], "incorrect block size in tier");
            uint16_t size = alloc->memory.size + alloc->block_size; // assumes overhead fits within one block
            ASSERT_EQUAL_INT(size, expected_size, "incorrect size in tier");

            uint8_t*expected_head = (uint8_t*)memory + expected_size * i;
            uint8_t* head = (uint8_t*)alloc->bitmaps.used;
            ASSERT_EQUAL_PTR(head, expected_head, "incorrect head in tier");
        }
    } CASE_COMPLETE;

    TEST_CASE("invalid tier allocator") {
        TierAllocator* allocator =  NULL;
        bool res = initTierAllocator(allocator, (void*)memory, MEMORY_SIZE);
        ASSERT_FALSE(res, "initialization succeeded");
    } CASE_COMPLETE;

    TEST_CASE("invalid memory") {
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        bool res = initTierAllocator(&allocator, NULL, MEMORY_SIZE);
        ASSERT_FALSE(res, "initialization succeeded");
    } CASE_COMPLETE;

    TEST_CASE("invalid size") {
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        bool res = initTierAllocator(&allocator, (void*)memory, 0);
        ASSERT_FALSE(res, "initialization succeeded");
    } CASE_COMPLETE;

    memset(memory, 0, MEMORY_SIZE);
}

void test_tierAllocate(void) {
    TEST_CASE("max sized allocation") {
        
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        (void*)initTierAllocator(&allocator, (void*)memory, MEMORY_SIZE);

        TEST_CASE("tier 1:") {
            void* block = tierAllocate(&allocator, 16);
            ASSERT_NOT_NULL(block, "tier allocation failed");
            BlockAllocator* alloc = &allocator.allocators[0];
            ASSERT_EQUAL_PTR(block, alloc->memory.head, "block not allocated in correct tier");
            ASSERT_EQUAL_INT(alloc->bitmaps.used[0], 1, "incorrect block size in tier");
        } CASE_COMPLETE;

        TEST_CASE("tier 2:") {
            void* block = tierAllocate(&allocator, 32);
            ASSERT_NOT_NULL(block, "tier allocation failed");
            BlockAllocator* alloc = &allocator.allocators[1];
            ASSERT_EQUAL_PTR(block, alloc->memory.head, "block not allocated in correct tier");
            ASSERT_EQUAL_INT(alloc->bitmaps.used[0], 1, "incorrect block size in tier");
        } CASE_COMPLETE;

        TEST_CASE("tier 3:") {
            void* block = tierAllocate(&allocator, 64);
            ASSERT_NOT_NULL(block, "tier allocation failed");
            BlockAllocator* alloc = &allocator.allocators[2];
            ASSERT_EQUAL_PTR(block, alloc->memory.head, "block not allocated in correct tier");
            ASSERT_EQUAL_INT(alloc->bitmaps.used[0], 1, "incorrect block size in tier");
        } CASE_COMPLETE;

        TEST_CASE("tier 4:") {
            void* block = tierAllocate(&allocator, 128);
            ASSERT_NOT_NULL(block, "tier allocation failed");
            BlockAllocator* alloc = &allocator.allocators[3];
            ASSERT_EQUAL_PTR(block, alloc->memory.head, "block not allocated in correct tier");
            ASSERT_EQUAL_INT(alloc->bitmaps.used[0], 0b01, "incorrect block size in tier");
        } CASE_COMPLETE;
    } CASE_COMPLETE;

    memset(memory, 0, MEMORY_SIZE);
    
    TEST_CASE("middle size allocation") {   
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        (void*)initTierAllocator(&allocator, (void*)memory, MEMORY_SIZE);
        
        void* block = tierAllocate(&allocator, 48); // should take 2x blocks for tier 2
        ASSERT_NOT_NULL(block, "tier allocation failed");
        BlockAllocator* alloc = &allocator.allocators[1];
        ASSERT_EQUAL_PTR(block, alloc->memory.head, "block not allocated in correct tier");
        ASSERT_EQUAL_INT(alloc->bitmaps.used[0], 0b11, "incorrect block size in tier");
    } CASE_COMPLETE;
    
    memset(memory, 0, MEMORY_SIZE);
    
}

void test_blockDeallocate(void) {
    TEST_CASE("max sized deallocation") { 
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        (void*)initTierAllocator(&allocator, (void*)memory, 4096);
        
        TEST_CASE("tier 1:"){
            void* block = tierAllocate(&allocator, 16);
            bool res = tierDeallocate(&allocator, block);
            ASSERT_TRUE(res, "deallocation failed");
        } CASE_COMPLETE;

        TEST_CASE("tier 2:"){
            void* block = tierAllocate(&allocator, 32);
            bool res = tierDeallocate(&allocator, block);
            ASSERT_TRUE(res, "deallocation failed");
        } CASE_COMPLETE;

        TEST_CASE("tier 3:"){
            void* block = tierAllocate(&allocator, 64);
            bool res = tierDeallocate(&allocator, block);
            ASSERT_TRUE(res, "deallocation failed");
        } CASE_COMPLETE;

        TEST_CASE("tier 4:"){
            void* block = tierAllocate(&allocator, 128);
            bool res = tierDeallocate(&allocator, block);
            ASSERT_TRUE(res, "deallocation failed");
        } CASE_COMPLETE;
    } CASE_COMPLETE;

    memset(memory, 0, MEMORY_SIZE);

    TEST_CASE("middle size deallocation") {
        TierAllocator allocator = {
            .block_sizes =  (uint16_t[]){16, 32, 64, 128},
            .tiers = 4,
            .allocators = (BlockAllocator[4]){}
        };
        (void*)initTierAllocator(&allocator, (void*)memory, 4096);
        
        void* block = tierAllocate(&allocator, 48); // should take 2x blocks for tier 2
        bool res = tierDeallocate(&allocator, block);
        ASSERT_TRUE(res, "deallocation failed");
    } CASE_COMPLETE;
}

int main(void) {
    LOG_INFO("ALLOCATOR TESTS\n");
    TEST_EVAL(test_initTierAllocator);
    TEST_EVAL(test_tierAllocate);
    TEST_EVAL(test_blockDeallocate);
    return testGetStatus();
}