#pragma once

#include "block_allocator.h"
#include <stdint.h>
#include <stdbool.h>

#define TIER_ALLOCATOR_OK 0
typedef struct {
    BlockAllocator* allocators;
    uint16_t* block_sizes;
    uint8_t tiers;
} TierAllocator;

int initTierAllocator(TierAllocator* allocator, void* memory, indexSize_t size);
void* tierAllocate(TierAllocator* allocator, indexSize_t size);
int tierDeallocate(TierAllocator* allocator, void* ptr);