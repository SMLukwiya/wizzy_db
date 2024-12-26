#ifndef PAGER_H
#define PAGER_H

#include "btree.h"

void load_node(uint64 offset, BNODE *);
void save_node(uint64 offset, BNODE *);

#endif