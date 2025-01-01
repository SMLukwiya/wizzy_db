#ifndef BTREE_H
#define BTREE_H

#include "mem.h"
#include <ctype.h>
#include <stdbool.h>

/* Block size 4KB */
#define BTREE_MAX_PAGE_SIZE 4096
#define BNODE_MAX_SIZE (BTREE_MAX_PAGE_SIZE - (sizeof(bool) + sizeof(void *)))
/* Degree of tree/Maximum number of children */
#define MAX_KEYS 200

typedef __uint8_t uint8;
typedef __uint64_t uint64;

typedef struct BNODE_INTERNAL {
    uint64 keys[MAX_KEYS - 1];
    uint64 child_offsets[MAX_KEYS];
} BNODE_INTERNAL;

typedef struct BNODE_LEAF {
    uint64 keys[MAX_KEYS];
    uint64 data_offsets[MAX_KEYS];
    /* Offset to next node */
    uint64 next_offset;
} BNODE_LEAF;

typedef struct BNODE {
    /* number of keys */
    uint8 numOfKeys;
    /* 0 for internal, 1 for leaf */
    uint8 isLeaf;
    /* Offset of current node in disk */
    uint64 offset;
    union {
        BNODE_INTERNAL bInternal;
        BNODE_LEAF bLeaf;
    };
} BNODE;

typedef struct BPTREE {
    /* Offset to root node on disk */
    uint64 root;
    /* Offset to next available/free block on disk */
    uint64 next_page_offset;

    /* APIS */
    int (*insert)(struct BPTREE *, uint64 key, uint64 value_offset);
    int (*del)(struct BPTREE *, uint64 key);
    uint64 (*search)(struct BPTREE *, uint64 key, bool *found);
} BPTREE;

/*  */
BNODE *create_node(SEGREGATED_LIST *list, bool isLeaf);
BPTREE *create_tree();
int insert(SEGREGATED_LIST *list, BPTREE *tree, uint64 key, uint64 offset);

#endif