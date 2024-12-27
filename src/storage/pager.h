#ifndef PAGER_H
#define PAGER_H

#include "btree.h"

int load_node(uint64 offset, BNODE *);
int save_node(BNODE *);

#endif