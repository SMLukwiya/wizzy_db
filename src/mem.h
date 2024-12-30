#ifndef __MEM_ALLOCATOR_H
#define __MEM_ALLOCATOR_H

#include <ctype.h>
#include <stdbool.h>

/* 99 MBs */
#define MAX_POOL 99 * (1 << 20)

#define ALIGNMENT 8

typedef __uint32_t uint32;

/* Block */
typedef struct BLOCK {
    bool isFree;
    struct BLOCK *next;
} BLOCK;

/* Segregated List */
typedef struct SEGREGATED_LIST {
    /* Pointer to original memory pool (for freeing) */
    void *memoryPoolPtr;
    /* Pointer to head of free list */
    BLOCK *freeListHeadPtr;
    /* Size of each block in the list */
    uint32 blockSize;
    /* Total number of blocks */
    uint32 totalBlocks;
    /* Number of free blocks */
    uint32 freeBlocks;
} SEGREGATED_LIST;
//

typedef struct MEMORY_POOL {
    // coalescing when blocks are free (reducing fragmentation)
    // Splitting larger blocks according to request
    // MEMORY LIMITS
    // - Max pool size
    // - Max Block Size
    // - Alignment (alignof())
    // - Stats -- #allocation, #delocations, cur mem usage, peak mem usage
    // - Thread Safety
    // Minimise memory frag (buddy allocation/segregated list)
} MEMORY_POOL;

/* APIS */
int seg_list_init(SEGREGATED_LIST *list, uint32 blockSize, uint32 numOfBlocks);
int extend_mem_pool(SEGREGATED_LIST *list);
BLOCK *allocate_block(SEGREGATED_LIST *list);
int deallocate_mem_pool(SEGREGATED_LIST *list);
int deallocate_block(SEGREGATED_LIST *list, BLOCK *block);

#endif