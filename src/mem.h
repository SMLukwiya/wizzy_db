#ifndef __MEM_ALLOCATOR_H
#define __MEM_ALLOCATOR_H

#include <ctype.h>
#include <stdbool.h>

/* 99 MBs */
#define MAX_POOL 99 * (1 << 20)

typedef __uint32_t uint32;

/* Block */
typedef struct BLOCK {
    bool isFree;
    struct BLOCK *next;
    /* node section */
    char data[];
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

/* APIS */
int seg_list_init(SEGREGATED_LIST *list, uint32 blockSize, uint32 numOfBlocks);
int extend_mem_pool(SEGREGATED_LIST *list);
void *allocate_block(SEGREGATED_LIST *list);
int deallocate_mem_pool(SEGREGATED_LIST *list);
void deallocate_block(SEGREGATED_LIST *list, BLOCK *block);

#endif