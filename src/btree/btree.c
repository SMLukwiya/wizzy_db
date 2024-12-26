#include "btree.h"
#include "pager.h"

#include <stdlib.h>

/* Create a new B+ tree node */
static BNODE *create_node(bool isLeaf) {
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

/* Split child node that is full */
BNODE *split_child(BPTREE *tree, BNODE *parent, BNODE *childNode, int index) {
    int i;

    BNODE *newNode = create_node(childNode->isLeaf);
    newNode->offset = tree->next_page_offset;

    int middleIndex = M / 2;
    /* Leaf nodes retain all keys after split
     * Internal nodes remove middle key from child and places in parent
     * Therefore one less key is retained
     */
    newNode->numOfKeys = newNode->isLeaf ? middleIndex : middleIndex - 1;
    /* move next_page_offset to next free space by M */
    tree->next_page_offset += 4096;

    /* Copy keys and child offsets to new node */
    for (i = 0; i < newNode->numOfKeys; i++) {
        if (newNode->isLeaf) {
            newNode->bLeaf.keys[i] = childNode->bLeaf.keys[i + middleIndex];
            newNode->bLeaf.data_offsets[i] = childNode->bLeaf.data_offsets[i + middleIndex];
        } else {
            newNode->bInternal.keys[i] = childNode->bInternal.keys[i + M / 2];
            newNode->bInternal.child_offsets[i] = childNode->bInternal.child_offsets[i + M / 2];
        }
    }
    childNode->numOfKeys = childNode->isLeaf ? middleIndex : middleIndex - 1;

    /* Update next node pointer for leaf node
     * new node points to whatever child node was pointing to
     * child node points to new node
     */
    if (newNode->isLeaf) {
        newNode->bLeaf.next_offset = childNode->bLeaf.next_offset;
        childNode->bLeaf.next_offset = newNode->offset;
    }

    /* shift parent node keys and child offsets to make room for new node */
    for (i = parent->numOfKeys; i > index; i--) {
        parent->bInternal.keys[i] = parent->bInternal.keys[i - 1];
        parent->bInternal.child_offsets[i + 1] = parent->bInternal.child_offsets[i];
    }
    /* Promote middle child key to parent key array */
    if (childNode->isLeaf) {
        parent->bInternal.keys[index] = childNode->bLeaf.keys[middleIndex];
    } else {
        parent->bInternal.keys[index] = childNode->bInternal.keys[middleIndex - 1];
    }
    parent->numOfKeys++;
    parent->bInternal.child_offsets[index + 1] = newNode->offset;

    /* Persist update to disk */
    save_node(childNode->offset, childNode);
    save_node(newNode->offset, newNode);
    save_node(parent->offset, parent);

    return newNode;
}

void insert_non_full(BPTREE *tree, BNODE **childNode, uint64 key, uint64 offset) {
    BNODE *node = *childNode;
    BNODE *newNode;
    int numOfKeys = node->numOfKeys;
    int i = numOfKeys;

    if (node->isLeaf) { // TODO: check numOfKeys from initial caller and split from caller if needed
        /* Shift node keys and data_offsets to make room for new key */
        while (i > 0 && node->bLeaf.keys[i - 1] > key) {
            node->bLeaf.keys[i] = node->bLeaf.keys[i - 1];
            node->bLeaf.data_offsets[i] = node->bLeaf.data_offsets[i - 1];
            i--;
        }
        node->bLeaf.keys[i] = key;
        node->bLeaf.data_offsets[i] = offset;
        node->numOfKeys++;
    } else {
        /* Search for correct position of child node */
        while (i > 0 && node->bInternal.keys[i - 1] > key) {
            i--;
        }

        uint64 childNodeOffset = node->bInternal.child_offsets[i];
        // BNODE *childNode;
        load_node(childNodeOffset, *childNode);
        int maxKeys = (*childNode)->isLeaf ? M : M - 1;

        /* split child */
        if ((*childNode)->numOfKeys == maxKeys) {
            newNode = split_child(tree, node, *childNode, i);

            /* Check if position in parent is correct after promoting middle child key */
            if (node->bInternal.keys[i] < key) {
                /* switch child node to point to correct node where insert should continue */
                *childNode = newNode;
            }
        }

        insert_non_full(tree, childNode, key, offset);

        /* If new node is created during current split
         * Free it only once
         */
        if (*childNode == newNode) {
            free(newNode);
        }
    }
}

/* Create B+ tree */
BPTREE *create_tree() {
    BPTREE *tree = (BPTREE *)malloc(sizeof(BPTREE));
    if (!tree) {
        perror("Failed to allocate memory for tree");
        exit(EXIT_FAILURE);
    }

    tree->root = 0;
    tree->next_page_offset = 0;

    return tree;
}