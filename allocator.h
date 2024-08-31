/**
 * @file allocator.h
 *
 * The allocator divides the memory pool into a series of fixed, equal-size blocks,
 * which is specified by the @c block_size parameter in the @ref initAllocator function. 
 * This block size determines the granularity of memory allocation, and all allocations 
 * will be an integer multiple of this block size. An allocation request for a block of 
 * size that is not a multiple of the block size will be rounded up to the next multiple.
 * 
 * For example, if the block size is set to 16 bytes, the allocator can only 
 * allocate memory blocks of size 16, 32, 48, 64, etc. bytes, and a request for 18 
 * bytes will be rounded up to 32.
 *
 * The allocator uses a pair of bitmaps to track the allocation status of each memory 
 * block in the pool. One bitmap tracks the used blocks, and the other bitmap tracks 
 * the allocated block heads. Each bitmap has an integer multiple of @c MAPSIZE bits, with the 
 * total size being determined by the number of blocks in the pool at initialization. 
 * This conditionally sized overhead allows for efficient memory usage, while maintaining
 * byte aligment in 2x @c MAPSIZE -bit systems.
 * 
 */
#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAPSIZE
#define MAPSIZE  32
#endif

#define MAPSIZE_MAX FWD_MAX(MAPSIZE)
#define FWD_MAX(arg) MAX_ARG(arg)
#define MAX_ARG(arg) UINT##arg##_MAX

#define MAPSIZE_TYPE FWD_TYPE(MAPSIZE)
#define FWD_TYPE(arg) TYPE_ARG(arg)
#define TYPE_ARG(arg) uint##arg##_t

typedef MAPSIZE_TYPE mapSize_t;

/**
 * @brief A block of memory with a pointer to its head and its size.
 */
typedef struct {
    void* head;     ///< Pointer to the start of the memory block.
    mapSize_t size;  ///< Size of the memory block.
} MemoryBlock;

/**
 * @brief Bitmaps for tracking used and allocated memory.
 */
typedef struct {
    mapSize_t* used;   ///< Bitmap tracking used blocks.
    mapSize_t* heads;  ///< Bitmap tracking allocated block heads.
    mapSize_t size;    ///< Size of the bitmap.
} BitMaps;

/**
 * @brief Represents an allocator with bitmaps and a memory block.
 */
typedef struct {
    BitMaps bitmaps;      ///< Bitmaps for managing memory allocation.
    MemoryBlock memory;   ///< The memory block being managed.
    mapSize_t block_size;  ///< Size of each memory block.
} Allocator;

/* -- Externs (avoid these for library functions) ------------------------- */

/* -- Function Declarations ----------------------------------------------- */

/**
 * @brief Initializes an allocator.
 * 
 * @param allocator The allocator to initialize.
 * @param block_size The size of each block.
 * @param memory The memory region to manage.
 * @param size The size of the memory region.
 * 
 * @note
 * The provided `memory` MUST point to a block of free, zero-initialized memory of size `size`.
 */
void initAllocator(Allocator* allocator, mapSize_t block_size, void* memory, mapSize_t size);

/**
 * @brief Allocates a block of memory from the allocator.
 * 
 * @param allocator The allocator to use for allocation.
 * @param size The size of the memory block to allocate in bytes.
 * @return A pointer to the allocated memory block, or NULL if the space is unavailable.
 */
void* allocate(Allocator* allocator, mapSize_t size);

/**
 * @brief Deallocates a previously allocated block of memory from the allocator.
 *
 * @param allocator The allocator to use for deallocation.
 * @param ptr A pointer to the start of the block of memory to be deallocated.
 *
 * @return true if the block was successfully deallocated, false otherwise.
 */
bool deallocate(Allocator* allocator, void* ptr);

#endif // _ALLOCATOR_H_
