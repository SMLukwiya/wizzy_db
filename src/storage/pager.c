#include "pager.h"

#include <stdio.h>

/* Read node from disk to node structure
 * starting at beginning of file
 */
void load_node(uint64 offset, BNODE *node) {
    const char *db = "./data/db";
    FILE *fd;
    fd = fopen(db, 'r');
    if (!fd) {
        perror("Database File not found");
        return;
    }

    fseek(fd, offset, SEEK_SET);
    fread(node, BTREE_MAX_PAGE_SIZE, 1, fd);
    fclose(fd);
}

void save_node(BNODE *node) {
    const char *db = "./data/db";
    FILE *fd = fopen(db, 'r+');
    if (!fd)
        fd = fopen(db, 'w+');
    if (!fd) {
        perror("Error settings up database");
        return;
    }

    fseek(fd, node->offset, SEEK_SET);
    fwrite(node, BTREE_MAX_PAGE_SIZE, 1, fd);
    fclose(fd);
}