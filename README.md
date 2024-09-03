![Coverage Status](https://github.com/nick-a-schneider/bitmap_allocator/actions/workflows/test.yaml/badge.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

**Memory Allocator Project Overview**
=====================================

This project implements a custom memory allocator that divides a memory block into a sequence of euqal-sized sub-blocks. The allocator is designed to manage memory allocation and deallocation, with a focus on optimizing overhead and maintaining byte alignment.

**Allocator Functionality**
---------------------------

The allocator works as follows:

* The memory pool is divided into fixed-size blocks, specified by the `block_size` parameter in the `initAllocator` function.
* All allocations are an integer multiple of the block size, with requests for non-multiple sizes being rounded up to the next multiple.
* A pair of bitmaps is used to track the allocation status of each memory block in the pool:
	+ One bitmap tracks used blocks.
	+ The other bitmap tracks allocated block heads.
* Each bitmap has an integer multiple of `MAPSIZE` bits, with the total size determined by the number of blocks in the pool at initialization. By default `MAPSIZE` is 16, but this macro can be redefined in the compiler tooling to either: `8`, `16`, or `32`. These values have been tested to preserve byte alignment in 16, 32, and 64bit systems.

**Example Usage**
-----------------

For example, if the block size is set to 16 bytes, the allocator can only allocate memory blocks of size 16, 32, 48, 64, etc. bytes. A request for 18 bytes will be rounded up to 32 (assuming default MAPSIZE). 

```c
// Initialize the allocator with a block size of 16 bytes
#include "allocator.h"
// ...
Allocator allocator;
uint8_t memory[128];
initAllocator(&allocator, 16, memory, 128);

// Allocate a block of 18 bytes (will be rounded up to 32 bytes, assuming default MAPSIZE)
void* block = allocate(&allocator, 18);

// Free the allocated block
deallocate(&allocator, block);
```