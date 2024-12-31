#include "btree.h"
#include "mem.h"
#include "test.h"

TEST_CASE(test_create_node) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 5);

    BNODE *node = create_node(&list, true);
    ASSERT_TRUE(node->isLeaf == 1, NULL);
    ASSERT_INT_EQUAL(list.freeBlocks, 4, NULL);
}

ADD_TEST(test_create_node);