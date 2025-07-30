#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAPSIZE
#define MAPSIZE  16
#endif

#ifndef INDEXSIZE
#define INDEXSIZE  32
#endif

#define MAPSIZE_MAX FWD_MAX(MAPSIZE)
#define FWD_MAX(arg) MAX_ARG(arg)
#define MAX_ARG(arg) UINT##arg##_MAX

#define MAPSIZE_TYPE FWD_TYPE(MAPSIZE)
#define INDEXSIZE_TYPE FWD_TYPE(INDEXSIZE)
#define FWD_TYPE(arg) TYPE_ARG(arg)
#define TYPE_ARG(arg) uint##arg##_t

typedef MAPSIZE_TYPE mapSize_t;
typedef INDEXSIZE_TYPE indexSize_t;

/**
 * @brief A block of memory with a pointer to its head and its size.
 */
typedef struct {
    void* head;         ///< Pointer to the start of the memory block.
    indexSize_t size;   ///< Size of the memory block.
} MemoryBlock;

/**
 * @brief Bitmaps for tracking used and allocated memory.
 */
typedef struct {
    mapSize_t* used;    ///< Bitmap tracking used blocks.
    mapSize_t* heads;   ///< Bitmap tracking allocated block heads.
    indexSize_t size;   ///< Size of the bitmap.
} BitMaps;

/**
 * @brief Represents an allocator with bitmaps and a memory block.
 */
typedef struct {
    BitMaps bitmaps;        ///< Bitmaps for managing memory allocation.
    MemoryBlock memory;     ///< The memory block being managed.
    indexSize_t block_size; ///< Size of each memory block.
} Allocator;

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
void initAllocator(Allocator* allocator, indexSize_t block_size, void* memory, indexSize_t size);

/**
 * @brief Allocates a block of memory from the allocator.
 * 
 * @param allocator The allocator to use for allocation.
 * @param size The size of the memory block to allocate in bytes.
 * @return A pointer to the allocated memory block, or NULL if the space is unavailable.
 */
void* allocate(Allocator* allocator, indexSize_t size);

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
