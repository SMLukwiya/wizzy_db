#include "pager.h"

#include <stdio.h>

/* Read node from disk to node structure
 * starting at beginning of file
 */
int load_node(uint64 offset, BNODE *node) {
    const char *db = "./data/db";
    FILE *fd = fopen(db, "rb");
    if (!fd) {
        perror("Failed to open database file");
        return -1;
    }

    if (fseek(fd, offset, SEEK_SET) != 0) {
        perror("Failed to move to node offset");
        fclose(fd);
        return -2;
    }

    if (fread(node, BTREE_MAX_PAGE_SIZE, 1, fd) != 1) {
        perror("Failed to read node from disk");
        fclose(fd);
        return -3;
    }

    fclose(fd);
    return 0;
}

void save_node(BNODE *node) {
    const char *db = "./data/db";
    FILE *fd = fopen(db, 'r+b');

    if (!fd) {
        perror("Failed to open database file for write action");
        return -1;
    }

    if (fseek(fd, node->offset, SEEK_SET) != 0) {
        perror("Failed to move node offset for write action");
        fclose(fd);
        return -1;
    }

    if (fwrite(node, BTREE_MAX_PAGE_SIZE, 1, fd) != 1) {
        perror("Failed to write node to disk");
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}