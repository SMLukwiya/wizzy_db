/* Simple Segregated List Implementation */

#include "mem.h"

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
    char *memoryPool = (char *)malloc(blockSize * numOfBlocks);
    if (!memoryPool) {
        perror("Failed to allocate memory pool");
        return -1;
    }
    list->freeListHeadPtr = (BLOCK *)memoryPool;

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