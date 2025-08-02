#pragma once

#include "block_allocator.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    BlockAllocator* allocators;
    uint16_t* block_sizes;
    uint8_t tiers;
} TierAllocator;

bool initTierAllocator(TierAllocator* allocator, void* memory, indexSize_t size);
void* tierAllocate(TierAllocator* allocator, indexSize_t size);
bool tierDeallocate(TierAllocator* allocator, void* ptr);