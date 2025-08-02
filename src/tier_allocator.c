#include "block_allocator.h"
#include "tier_allocator.h"

bool initTierAllocator(TierAllocator* allocator, void* memory, indexSize_t size) {
    if (!allocator || !memory || !size) return false;
    if (!allocator->tiers || !allocator->block_sizes || !allocator->allocators) return false;

    indexSize_t raw_tier_size = size / allocator->tiers;
    indexSize_t tier_size = raw_tier_size & ~(sizeof(mapSize_t) - 1);
    if (tier_size == 0) return false;

    for (uint8_t i = 0; i < allocator->tiers; i++) {
        uint8_t* head = (uint8_t*)memory + tier_size * i;
        bool res = initBlockAllocator(&allocator->allocators[i], allocator->block_sizes[i], head, tier_size);
        if (!res) {
             return false;
        }
    }
    return true;
}

void* tierAllocate(TierAllocator* allocator, indexSize_t size) {
    if (!allocator) return NULL;
    if (!size) return NULL;
    for (uint8_t i = 0; i < allocator->tiers; i++) {
        if (size <= allocator->block_sizes[i] ||
            (i != allocator->tiers - 1 && size < allocator->block_sizes[i + 1])) {
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

// TierAllocator allocator = {
//     .block_sizes =  (uint16_t[]){16, 32, 64, 128},
//     .tiers = 4,
//     .allocators = (Allocator[4]){}
// };