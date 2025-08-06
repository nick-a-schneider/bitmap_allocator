#include "block_allocator.h"
#include "tier_allocator.h"
#include <errno.h>

int initTierAllocator(TierAllocator* allocator, void* memory, indexSize_t size) {
    if (!allocator || !memory || !size) return -EINVAL;
    if (!allocator->tiers || !allocator->block_sizes || !allocator->allocators) return -EBADF;

    indexSize_t raw_tier_size = size / allocator->tiers;
    indexSize_t tier_size = raw_tier_size & ~(sizeof(mapSize_t) - 1);
    if (tier_size == 0) return -ENOMEM;

    for (uint8_t i = 0; i < allocator->tiers; i++) {
        uint8_t* head = (uint8_t*)memory + tier_size * i;
        int res = initBlockAllocator(&allocator->allocators[i], allocator->block_sizes[i], head, tier_size);
        if (res != BLOCK_ALLOCATOR_OK) return res;
    }
    return TIER_ALLOCATOR_OK;
}

void* tierAllocate(TierAllocator* allocator, indexSize_t size) {
    if (!allocator || !size) return NULL;
    for (uint8_t i = 0; i < allocator->tiers; i++) {
        if (size <= allocator->block_sizes[i] ||
            (i != allocator->tiers - 1 && size < allocator->block_sizes[i + 1])) {
            return blockAllocate(&allocator->allocators[i], size);
        }
    }
    return NULL;
}

int tierDeallocate(TierAllocator* allocator, void* ptr) {
    if (!allocator || !ptr) return -EINVAL;
    for (uint8_t i = 0; i < allocator->tiers; i++) {
        int res = blockDeallocate(&allocator->allocators[i], ptr);
        if (res == BLOCK_ALLOCATOR_OK) break;
        if (res == -EFAULT) continue;
        return res;
    }
    return TIER_ALLOCATOR_OK;
}
