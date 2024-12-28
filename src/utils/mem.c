/* Simple Segregated List Implementation */

#include "mem.h"
#include "btree.h"

#include <stdlib.h>

/* Initialize Seg list */
int seg_list_init(SEGREGATED_LIST *list, int blockSize, int numOfBlocks) {
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

    if (list->freeBlocks == 0) {
        // extend block by a factor later
        return NULL;
    }

    /* Allocate block and update free list */
    BLOCK *block = list->freeListHeadPtr;
    block->isFree = 0;
    list->freeListHeadPtr = block->next;
    list->freeBlocks--;

    return block;
}

void *deallocate_block(SEGREGATED_LIST *list, BLOCK *block) {
    block->isFree = 1;

    block->next = list->freeListHeadPtr;
    list->freeListHeadPtr = block;
    list->freeBlocks++;
}