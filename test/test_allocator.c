#include "../allocator.h"
#include "test_utils.h"
#include <stdbool.h>

// recreation of private function for test purposes
bool get_bit(mapSize_t* bitmap, indexSize_t index) {
    return (bitmap[index / MAPSIZE] & (1ULL << (index % MAPSIZE))) != 0;
}

void testInitAllocator(void) {

    TEST_CASE("Even multiple of block size") {
        Allocator allocator;
        uint8_t memory[128];
        initAllocator(&allocator, 16, memory, 128);

        ASSERT_EQUAL_INT(allocator.memory.size, 128 - 16, "incorrect size after initialization");
        ASSERT_EQUAL_INT(allocator.block_size, 16, "incorrect block size after initialization");
        ASSERT_EQUAL_INT(allocator.bitmaps.size, 7, "incorrect bitmap size after initialization"); 

        ASSERT_EQUAL_PTR((uint8_t*)(allocator.bitmaps.used), memory, "heads bitmap placement is wrong"); 
        ASSERT_EQUAL_PTR((uint8_t*)(allocator.bitmaps.heads), memory + sizeof(mapSize_t), "used bitmap placement is wrong");  
        ASSERT_EQUAL_PTR((uint8_t*)(allocator.memory.head), memory + 2 * sizeof(mapSize_t), "heads bitmap placement is wrong"); 

    } CASE_COMPLETE;

    TEST_CASE("bitmaps fill memory block") {
        // TODO: test for case where 2x 16bit bitmaps exceed the size of the memory block
    } CASE_NOT_IMPLEMENTED;

    TEST_CASE("head aligned correctly") {
        Allocator allocator;
        uint8_t memory[4096];
        for (uint16_t block_size = 64; block_size  > 0; block_size--) {
            initAllocator(&allocator, block_size, memory, 4096);
            ASSERT_TRUE(((uintptr_t)(allocator.memory.head)) % (2 * sizeof(mapSize_t)) == 0, "head not aligned for block size %d", block_size);
        }
    } CASE_COMPLETE;

}

void testAllocate() {

    TEST_CASE("allocating entire memory block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, 16, memory, 128);
        void* block = allocate(&allocator, allocator.memory.size);
        ASSERT_EQUAL_PTR(block, allocator.memory.head, "incorrect block head returned");
        void* block2 = allocate(&allocator, 16);
        ASSERT_EQUAL_PTR(block2, NULL, "invalid allocation returned non-null");
        ASSERT_TRUE(get_bit(allocator.bitmaps.heads, 0), "heads bit not set");
        for (uint16_t i = 0; i < allocator.bitmaps.size; i++) {
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, i), "used bit[%d] was not set", i);
        }
    } CASE_COMPLETE;
        
    TEST_CASE("allocating impossibly large block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, 16, memory, 128);

        void* block = allocate(&allocator, 1024);
        ASSERT_EQUAL_PTR(block, NULL, "invalid allocation returned non-null");

        for (uint16_t i = 0; i < allocator.bitmaps.size; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.heads, i), "heads bit[%d] was set", i);
            ASSERT_FALSE(get_bit(allocator.bitmaps.used, i),  "used bit[%d] was set",  i);
        }
    } CASE_COMPLETE;

    TEST_CASE("allocating block that spans two bitmap blocks") {
        Allocator allocator;
        uint8_t memory[4 * MAPSIZE];
        uint16_t offset = 0;

        for (int index = 0; index < (4 * MAPSIZE); index++) memory[index] = 0;
        initAllocator(&allocator, 1, memory, 4 * MAPSIZE);

        void* block1 = allocate(&allocator, (3 * MAPSIZE) / 2);
        void* block2 = allocate(&allocator, MAPSIZE);

        ASSERT_NOT_EQUAL_PTR(block1, NULL, "valid allocation returned null");
        ASSERT_NOT_EQUAL_PTR(block2, NULL, "valid allocation returned null");

        ASSERT_TRUE(get_bit(allocator.bitmaps.heads, 0), "block1: heads bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 0), "block1: used bit not set");

        for (uint16_t i = 1; i < ((3 * MAPSIZE) / 2); i++) {
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, i), "block1: used bit[%d] not set", i);
            ASSERT_FALSE(get_bit(allocator.bitmaps.heads, i), "block1: heads bit[%d] was set", i);
        }
        ASSERT_TRUE(get_bit(allocator.bitmaps.heads, ((3 * MAPSIZE) / 2)), "block2: heads bit not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, ((3 * MAPSIZE) / 2)), "block2: used bit not set");

        for (uint16_t i = 1; i < MAPSIZE; i++) {
            offset = ((3 * MAPSIZE) / 2);
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, offset + i), "block2: used bit[%d] not set", i);
            ASSERT_FALSE(get_bit(allocator.bitmaps.heads, offset + i), "block2: heads bit[%d] was set", i);
        }
        for (uint16_t i = ((3 * MAPSIZE) / 2) + MAPSIZE; i < 4 * MAPSIZE; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.used, i), "free: used bit[%d] was set", i);
            ASSERT_FALSE(get_bit(allocator.bitmaps.heads, i), "free: heads bit[%d] was set", i);
        }

    } CASE_COMPLETE;

    TEST_CASE("allocating odd sized block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, 16, memory, 128);

        void* block1 = allocate(&allocator, 17);
        void* block2 = allocate(&allocator, 17);
        ASSERT_NOT_EQUAL_PTR(block1, NULL, "valid allocation returned null");
        ASSERT_NOT_EQUAL_PTR(block2, NULL, "valid allocation returned null");

        ASSERT_EQUAL_PTR((uint8_t*)block1 + 32, (uint8_t*)block2, "blocks not adjacent");

        ASSERT_TRUE(get_bit(allocator.bitmaps.heads, 0), "heads bit[0] not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.heads, 2), "heads bit[2] not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 0), "used bit[0] not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 1), "used bit[1] not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 2), "used bit[2] not set");
        ASSERT_TRUE(get_bit(allocator.bitmaps.used, 3), "used bit[3] not set");

    } CASE_COMPLETE;
}

void testDeallocate() {

    TEST_CASE("deallocating block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, 16, memory, 128);

        void* block = allocate(&allocator, 16);
        ASSERT_TRUE(deallocate(&allocator, block), "deallocation failed");

        ASSERT_FALSE(get_bit(allocator.bitmaps.heads, 0), "heads bit was set");
        ASSERT_FALSE(get_bit(allocator.bitmaps.used, 0), "used bit was set");

    } CASE_COMPLETE;

    TEST_CASE("deallocating block that spans two 16bit words in the bitmap") {
        Allocator allocator;
        uint8_t memory[4096];
        for (int index = 0; index < 4096; index++) memory[index] = 0;
        initAllocator(&allocator, 16, memory, 4096);

        void* block1 = allocate(&allocator, 16 * 12);
        void* block2 = allocate(&allocator, 16 * 20);

        ASSERT_TRUE(deallocate(&allocator, block1), "deallocating block1 failed");

        ASSERT_FALSE(get_bit(allocator.bitmaps.heads, 0), "heads bit still set after deallocation");
        for (uint16_t i = 0; i < 12; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.used, i), "used bit[%d] still set after deallocation", i);
        }

        ASSERT_TRUE(get_bit(allocator.bitmaps.heads, 12), "next allocs bit cleared after deallocation");
        for (uint16_t i = 0; i < 20; i++) {
            ASSERT_TRUE(get_bit(allocator.bitmaps.used, 12 + i), "next used bit[%d] cleared after deallocation", i);
        }

        ASSERT_TRUE(deallocate(&allocator, block2), "deallocating block2 failed");

    } CASE_COMPLETE;

    TEST_CASE("deallocating invalid block") {
        Allocator allocator;
        uint8_t memory[128];
        for (int index = 0; index < 128; index++) memory[index] = 0;
        initAllocator(&allocator, 16, memory, 128);
        void* block = (uint8_t*)(allocator.memory.head) + 2;

        ASSERT_FALSE(deallocate(&allocator, block), "invalid block was deallocation without error");
        for (uint16_t i = 0; i < 8; i++) {
            ASSERT_FALSE(get_bit(allocator.bitmaps.heads, i), "heads bit[%d] was cleared during invalid deallocation", i);
        }
    } CASE_COMPLETE;
}

int main(void) {
    LOG_INFO("ALLOCATOR TESTS\n");
    TEST_EVAL(testInitAllocator);
    TEST_EVAL(testAllocate);
    TEST_EVAL(testDeallocate);
    return testGetStatus();
}