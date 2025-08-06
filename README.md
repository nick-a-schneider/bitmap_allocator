<!-- ![Codecov](https://codecov.io/gh/nick-a-schneider/bitmap_allocator/branch/main/graph/badge.svg) -->
<!-- ![Coveralls](https://coveralls.io/repos/github/nick-a-schneider/bitmap_allocator/badge.svg?branch=main) -->
![Coverage Status](https://github.com/nick-a-schneider/bitmap_allocator/actions/workflows/test.yaml/badge.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Maintenance](https://img.shields.io/badge/Maintained%3F-yes-green.svg)
<!-- ![Contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg) -->


**Memory Allocator Project Overview**
=====================================

This project implements a custom memory allocator that divides a memory block into a sequence of euqal-sized sub-blocks. The allocator is designed to manage memory allocation and deallocation, with a focus on optimizing overhead and maintaining byte alignment.

**Allocator Functionality**
---------------------------

The block allocator works as follows:

* The memory pool is divided into fixed-size blocks, specified by the `block_size` parameter in the `initBlockAllocator` function.
* All allocations are an integer multiple of the block size, with requests for non-multiple sizes being rounded up to the next multiple.
* A pair of bitmaps is used to track the allocation status of each memory block in the pool:
	+ One bitmap tracks used blocks.
	+ The other bitmap tracks allocated block heads.
* Each bitmap has an integer multiple of `MAPSIZE` bits, with the total size determined by the number of blocks in the pool at initialization. By default `MAPSIZE` is 16, but this macro can be redefined in the compiler tooling to either: `8`, `16`, or `32`. These values have been tested to preserve byte alignment in 16, 32, and 64bit systems.

The tier allocator can partition a section of memory up into *N* blocks, which are each assigned their own block allocator. 

**Example Usage**
=================

**Block Allocator**
-------------------

For example, if the block size is set to 16 bytes, the allocator can only allocate memory blocks of size 16, 32, 48, 64, etc. bytes. A request for 18 bytes will be rounded up to 32 (assuming default MAPSIZE). 

```c
// Initialize the allocator with a block size of 16 bytes
#include "block_allocator.h"
// ...
BlockAllocator allocator;
uint8_t memory[128];
// ...
int res = initBlockAllocator(&allocator, 16, memory, 128);
// ...
// Allocate a block of 18 bytes (will be rounded up to 32 bytes, assuming default MAPSIZE)
void* block = blockAllocate(&allocator, 18);
// ...
// Free the allocated block
res = blockDeallocate(&allocator, block);
```

**Tier Allocator**
--------------------

For example: gievn a 4096 byte section of memory, if you choose the block sizes to be 16, 32, 64, and 128, the memory region will be split into four sections of 1024 bytes. Each 1024 byte will be managed by a block allocator with the corresponding size. 
```c
#include "tier_allocator.h"
// ...
uint8_t memory[4096];
TierAllocator allocator = {
	.block_sizes =  (uint16_t[]){16, 32, 64, 128},
	.tiers = 4,
	.allocators = (BlockAllocator[4]){}
};
// ...
int res = initTierAllocator(&allocator, (void*)memory, 4096);
// ...
// allocated from .allocators[0]
void* block_16  = tierAllocate(&allocator, 16);
// allocated from .allocators[1]
void* block_32  = tierAllocate(&allocator, 32);
// allocated from .allocators[2]
void* block_64  = tierAllocate(&allocator, 64);
// allocated from .allocators[3]
void* block_128 = tierAllocate(&allocator, 128);
// ...
```