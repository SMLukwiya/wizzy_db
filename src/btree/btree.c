#include "btree.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

static BNODE *createNode(bool isLeaf) {
    int i;
    BNODE *newNode = (BNODE *)malloc(sizeof(BNODE));
    if (!newNode) {
        perror("Failed to allocate newNode memory");
        exit(-1);
    }

    newNode->numOfKeys = (size_key)0;
    newNode->isLeaf = isLeaf;

    for (i = 0; i < M; i++) {
        newNode->children[i] = NULL;
    }

    return newNode;
}