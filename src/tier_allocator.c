#include "block_allocator.h"
#include "tier_allocator.h"

void initTierAllocator(TierAllocator* allocator, void* memory, indexSize_t size) {
    if (!allocator) return;
    if (!memory) return;
    if (!size) return;
    if (!allocator->tiers) return;
    if (!allocator->block_sizes) return;
    if (!allocator->allocators) return;

    indexSize_t tier_size = size / allocator->tiers;
    for (uint8_t i = 0; i < allocator->tiers; i++) {
        uint8_t* head = (uint8_t*)memory + tier_size * i;
        initBlockAllocator(&allocator->allocators[i], allocator->block_sizes[i], head, tier_size);
    }
    return;
}

void* tierAllocate(TierAllocator* allocator, indexSize_t size) {
    if (!allocator) return NULL;
    if (!size) return NULL;
    for (uint8_t i = 0; i < allocator->tiers; i++) {
        if (size <= allocator->block_sizes[i]) {
            return blockAllocate(&allocator->allocators[i], size);
        }
    }
    return NULL;
}

bool tierDeallocate(TierAllocator* allocator, void* ptr) {
    if (!allocator) return false;
    if (!ptr) return false;
    for (uint8_t i = 0; i < allocator->tiers; i++) {
        if (blockDeallocate(&allocator->allocators[i], ptr)) return true;
    }
    return false;
}

TierAllocator allocator = {
    .block_sizes =  (uint16_t[]){16, 32, 64, 128},
    .tiers = 4,
    .allocators = (Allocator[4]){}
};