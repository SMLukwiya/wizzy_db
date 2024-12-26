#ifndef BTREE_H
#define BTREE_H

#include <ctype.h>
#include <stdbool.h>

/* Page/Block size 4KB */
#define BTREE_MAX_PAGE_SIZE 4096
/* Degree of tree/Maximum number of children */
#define M 200

typedef __uint8_t uint8;
typedef __uint64_t uint64;

typedef struct BNODE_INTERNAL {
    uint64 keys[M - 1];
    uint64 child_offsets[M];
} BNODE_INTERNAL;

typedef struct BNODE_LEAF {
    uint64 keys[M];
    uint64 data_offsets[M];
    /* Offset to next node */
    uint64 next_offset;
} BNODE_LEAF;

typedef struct BNODE {
    /* grouping 1 bytes together could minimize padding due to alignment constraints */
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
    int (*insert)(struct BTREE *, uint64 key, uint64 value_offset);
    int (*del)(struct BTREE *, uint64 key);
    uint64 (*search)(struct BTREE *, uint64 key, bool *found);
} BTREE;

#endif