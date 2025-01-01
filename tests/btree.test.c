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

TEST_CASE(test_create_tree) {
    BPTREE *tree = create_tree();
    ASSERT_EQUAL(tree->root, 0, NULL);
    ASSERT_EQUAL(tree->next_page_offset, 0, NULL);
}

TEST_CASE(test_insert_key_empty) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 5);
    BPTREE *tree = create_tree();

    insert(&list, tree, 1, 1);
}

ADD_TEST(test_create_node);
ADD_TEST(test_create_tree);