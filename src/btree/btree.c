#include "btree.h"

#include <stdlib.h>

static BNODE *createNode(bool isLeaf) {
    int i;
    BNODE *newNode = (BNODE *)malloc(sizeof(BNODE));
    if (!newNode) {
        perror("Failed to allocate memory for newNode");
        exit(EXIT_FAILURE);
    }

    newNode->numOfKeys = 0;
    newNode->isLeaf = isLeaf;
    newNode->offset = 0;

    /* Zero out keys and children offsets */
    if (isLeaf) {
        for (i = 0; i < M; i++) {
            newNode->bLeaf.keys[i] = 0;
            newNode->bLeaf.data_offsets[i] = 0;
        }
        newNode->bLeaf.next_offset = 0;
    } else {
        for (i = 0; i < M - 1; i++) {
            newNode->bInternal.keys[i] = 0;
        }
        for (i = 0; i < M; i++) {
            newNode->bInternal.child_offsets[i] = 0;
        }
    }

    return newNode;
}