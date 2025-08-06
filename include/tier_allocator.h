#pragma once

#include "block_allocator.h"
#include <stdint.h>
#include <stdbool.h>

#define TIER_ALLOCATOR_OK 0 // success

/**
 * @brief Configuration for a tiered allocator.
 */
typedef struct {
    BlockAllocator* allocators;     ///< Array of internal BlockAllocators
    uint16_t* block_sizes;          ///< Array of supported block sizes
    uint8_t tiers;                  ///< Number of internal BlockAllocators
} TierAllocator;

/**
 * @brief Initializes a tiered allocator.
 * 
 * @param allocator The TierAllocator instance to initialize.
 * @param memory A block of memory used to configure and back the internal BlockAllocators.
 * @param size The total size of the memory region.
 * 
 * @note
 * The provided TierAllocator must be fully defined before calling this function, and `memory` 
 * MUST point to a block of free, zero-initialized memory of size `size`.
 * 
 * @return `TIER_ALLOCATOR_OK` on success, or a negative errno value on failure:
 * - `-EINVAL` if any required argument is NULL or zero
 * - `-EBADF` if the TierAllocator's tier configuration is invalid
 * - `-ENOMEM` if each tier's memory slice is too small
 * - Any error propagated from `initBlockAllocator`
 */
int initTierAllocator(TierAllocator* allocator, void* memory, indexSize_t size);

/**
 * @brief Allocates a block of memory from the tiered allocator.
 * 
 * @param allocator The TierAllocator to use for allocation.
 * @param size The size of the memory block to allocate in bytes.
 * 
 * @return A pointer to the allocated memory block, or NULL if no suitable tier has space
 *         or if the size exceeds all supported block sizes.
 */
void* tierAllocate(TierAllocator* allocator, indexSize_t size);

/**
 * @brief Deallocates a previously allocated block of memory from the tiered allocator.
 * 
 * @param allocator The TierAllocator to use for deallocation.
 * @param ptr A pointer to the memory block to deallocate.
 * 
 * @return errno: [EINVAL, EFAULT, TIER_ALLOCATOR_OK]
 */
int tierDeallocate(TierAllocator* allocator, void* ptr);