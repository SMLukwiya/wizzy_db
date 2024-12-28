#include "btree.h"
#include "pager.h"

#include <stdlib.h>
#include <string.h>

/* Create a new B+ tree node */
static BNODE *create_node(bool isLeaf) {
    int i;
    BNODE *newNode = (BNODE *)malloc(sizeof(BNODE));
    if (!newNode) {
        perror("Failed to allocate memory for newNode");
        return NULL;
    }

    /* Zero out the memory */
    memset(newNode, 0, BTREE_MAX_PAGE_SIZE);

    newNode->isLeaf = isLeaf;

    return newNode;
}

/* Split child node that is full */
BNODE *split_child(BPTREE *tree, BNODE *parent, BNODE *childNode, int index) {
    int i;

    BNODE *newNode = create_node(childNode->isLeaf);
    if (!newNode) {
        perror("Failed to allocate memory for new node");
        return NULL;
    }

    newNode->offset = tree->next_page_offset;

    int middleIndex = MAX_KEYS / 2;
    /* Leaf nodes retain all keys after split
     * Internal nodes remove middle key from child and places in parent
     * Therefore one less key is retained
     */
    newNode->numOfKeys = newNode->isLeaf ? middleIndex : middleIndex - 1;
    /* move next_page_offset to next free space by M */
    tree->next_page_offset += BTREE_MAX_PAGE_SIZE;

    /* Copy keys and child offsets to new node */
    for (i = 0; i < newNode->numOfKeys; i++) {
        if (newNode->isLeaf) {
            newNode->bLeaf.keys[i] = childNode->bLeaf.keys[i + middleIndex];
            newNode->bLeaf.data_offsets[i] = childNode->bLeaf.data_offsets[i + middleIndex];
        } else {
            newNode->bInternal.keys[i] = childNode->bInternal.keys[i + MAX_KEYS / 2];
            newNode->bInternal.child_offsets[i] = childNode->bInternal.child_offsets[i + MAX_KEYS / 2];
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
    if (save_node(childNode) != 0) {
        free(newNode);
        return NULL;
    }
    if (save_node(newNode) != 0) {
        free(newNode);
        return NULL;
    }
    if (save_node(parent) != 0) {
        free(newNode);
        return NULL;
    }

    return newNode;
}

int insert_non_full(BPTREE *tree, BNODE **childNode, uint64 key, uint64 offset) {
    BNODE *node = *childNode;
    BNODE *newNode = NULL;
    int numOfKeys = node->numOfKeys;
    int i = numOfKeys;

    if (node->isLeaf) {
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
        if (load_node(childNodeOffset, *childNode) != 0) {
            return -1;
        }
        int maxKeys = (*childNode)->isLeaf ? MAX_KEYS : MAX_KEYS - 1;

        /* split child */
        if ((*childNode)->numOfKeys == maxKeys) {
            newNode = split_child(tree, node, *childNode, i);
            if (!newNode) {
                return -1;
            }

            /* Check if position in parent is correct after promoting middle child key */
            if (node->bInternal.keys[i] < key) {
                /* switch child node to point to correct node where insert should continue */
                *childNode = newNode;
            }
        }

        int result = insert_non_full(tree, childNode, key, offset);
        if (result != 0) {
            return result;
        }
    }

    /* Free new node if it was created during current split */
    if (newNode != NULL) {
        free(newNode);
    }

    return 0;
}

int insert(BPTREE *tree, uint64 key, uint64 offset) {
    uint64 rootOffset = tree->root;
    BNODE *node;

    /* If tree is empty, insert and return */
    if (rootOffset == 0) {
        node = create_node(true);
        if (!node) {
            return -1;
        }
        node->bLeaf.keys[0] = key;
        node->bLeaf.data_offsets[0] = offset;
        node->numOfKeys = 1;
        node->offset = tree->next_page_offset;
        tree->next_page_offset += BTREE_MAX_PAGE_SIZE;
        tree->root = node->offset;

        /* Persist to disk */
        if (save_node(node) != 0) {
            free(node);
            return -2;
        }

        /* node is part of tree, therefore no call to free */
        return 0;
    }

    node = (BNODE *)malloc(BTREE_MAX_PAGE_SIZE);
    if (!node) {
        return -1;
    }
    if (load_node(rootOffset, node) != 0) {
        free(node);
        return -2;
    }

    /* Check if rootNode is full */
    if (node->numOfKeys == MAX_KEYS - 1) {
        /* Create new root to hold promoted key */
        BNODE *newRoot = create_node(false);
        if (!newRoot) {
            free(node);
            return -1;
        }

        newRoot->offset = tree->next_page_offset;
        newRoot->bInternal.child_offsets[0] = node->offset;
        tree->next_page_offset += BTREE_MAX_PAGE_SIZE;
        tree->root = newRoot->offset;
        BNODE *newNode = split_child(tree, newRoot, node, 0);
        if (!newNode) {
            free(newRoot);
            free(node);
            return -1;
        }

        /* Check which child node should receive new key */
        if (newRoot->bInternal.keys[0] < key) {
            free(node);
            node = newNode;
        } else {
            free(newNode);
        }
    }

    insert_non_full(tree, &node, key, offset);
    if (save_node(node) != 0) {
        free(node);
        return -2;
    }

    free(node);
    return 0;
}

int search_node(BNODE *startNode, uint64 key, BNODE **returnNode) {

    if (startNode->isLeaf) {
        int left = 0, right = startNode->numOfKeys - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (startNode->bLeaf.keys[mid] == key) {
                uint64 offset = startNode->bLeaf.data_offsets[mid];
                if (load_node(offset, *returnNode) != 0) {
                    return -1;
                }
                return 0;
            } else if (startNode->bLeaf.keys[mid] < key) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1;
    }

    int i = startNode->numOfKeys - 1;

    /* Get correct child node position to search */
    while (i > 0 && startNode->bInternal.keys[i - 1] > key) {
        i--;
    }

    uint64 childOffset = startNode->bInternal.child_offsets[i];
    BNODE *childNode = (BNODE *)malloc(BTREE_MAX_PAGE_SIZE);
    if (load_node(childOffset, startNode) != 0) {
        return -1;
    }

    return search_node(startNode, key, returnNode);
}

int delete(BPTREE *tree, uint64 key) {
    uint64 rootOffset = tree->root;
    if (rootOffset == 0) {
        return 0;
    }

    BNODE *node = (BNODE *)malloc(BTREE_MAX_PAGE_SIZE);
    if (load_node(rootOffset, node) != 0) {
        free(node);
        return -1;
    }

    uint64 nodeOffset;
    int i;
    if (node->isLeaf) {
        //
        for (i = 0; i < node->numOfKeys; i++) {
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