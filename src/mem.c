/* Simple Segregated List Implementation */

#include "mem.h"
#include "btree.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Initialize Seg list */
int seg_list_init(SEGREGATED_LIST *list, uint32 blockSize, uint32 numOfBlocks) {
    /* Validate inputs */
    if (blockSize < 1 || numOfBlocks < 1 || !list) {
        return -1;
    }

    list->blockSize = blockSize;
    list->totalBlocks = numOfBlocks;
    list->freeBlocks = numOfBlocks;

    /* Allocate memory */
    list->memoryPoolPtr = malloc(blockSize * numOfBlocks);
    if (!list->memoryPoolPtr) {
        perror("Failed to allocate memory pool");
        return -1;
    }
    list->freeListHeadPtr = (BLOCK *)list->memoryPoolPtr;

    /* Initialize blocks */
    BLOCK *currentBlock = list->freeListHeadPtr;

    for (int i = 0; i < numOfBlocks; i++) {
        currentBlock->isFree = 1;
        if (i < numOfBlocks - 1) {
            currentBlock->next = (BLOCK *)((char *)currentBlock + blockSize);
        } else {
            currentBlock->next = NULL;
        }
        currentBlock = currentBlock->next;
    }
    return 0;
}

/* Free memory pool */
int deallocate_mem_pool(SEGREGATED_LIST *list) {
    if (!list || !list->freeListHeadPtr) {
        return -1;
    }

    free(list->memoryPoolPtr);

    /* Reset seg list */
    list->blockSize = 0;
    list->freeBlocks = 0;
    list->totalBlocks = 0;
    list->memoryPoolPtr = NULL;
    list->freeListHeadPtr = NULL;
    return 0;
}

/* Allocate block */
BLOCK *allocate_block(SEGREGATED_LIST *list) {
    if (!list->freeListHeadPtr) {
        return NULL;
    }

    /* Try to extend memory pool */
    if (list->freeBlocks == 0) {
        if (extend_mem_pool(list) != 0) {
            return NULL;
        }
    }

    /* Allocate block and update free list */
    BLOCK *block = list->freeListHeadPtr;
    block->isFree = 0;
    list->freeListHeadPtr = block->next;
    list->freeBlocks--;

    return block;
}

/* Deallocate block and update free list */
void deallocate_block(SEGREGATED_LIST *list, BLOCK *block) {
    block->isFree = 1;

    block->next = list->freeListHeadPtr;
    list->freeListHeadPtr = block;
    list->freeBlocks++;
}

/* Expands memory x2 */
int extend_mem_pool(SEGREGATED_LIST *list) {
    uint32 currentSize = list->blockSize * list->totalBlocks;
    if (currentSize >= MAX_POOL) {
        return -1;
    }

    /* Extend to max pool currentSize */
    uint32 newSize = (currentSize * 2) > MAX_POOL ? MAX_POOL : currentSize * 2;
    /* New added number of blocks */
    int addedBlocks = (newSize - currentSize) / list->blockSize;

    /* Reallocate new memory pool */
    void *newMemoryPool = realloc(list->memoryPoolPtr, newSize);
    if (!newMemoryPool) {
        return -1;
    }

    /* Update free list to add new blocks */
    BLOCK *firstBlockOnOldList = (BLOCK *)((char *)newMemoryPool + currentSize);

    /* Initialize new blocks */
    BLOCK *currentBlock = firstBlockOnOldList;

    for (int i = 0; i < addedBlocks; i++) {
        currentBlock->isFree = 1;
        if (i < addedBlocks - 1) {
            currentBlock->next = (BLOCK *)((char *)currentBlock + list->blockSize);
        } else {
            currentBlock->next = NULL;
        }
        currentBlock = currentBlock->next;
    }

    /* Update seg list */
    list->memoryPoolPtr = newMemoryPool;
    /* Since we call it when we exhaust current pool
     * We can point free list to the first new block;
     */
    list->freeListHeadPtr = firstBlockOnOldList;
    list->freeBlocks += addedBlocks;
    list->totalBlocks += addedBlocks;
    return 0;
}