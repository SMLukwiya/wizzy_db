#include "mem.h"
#include "test.h"

TEST_CASE(test_seg_list_init) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 10);

    // TODO: message field optional
    ASSERT_INT_EQUAL(list.blockSize, 4096, NULL);
    ASSERT_INT_EQUAL(list.totalBlocks, 10, NULL);
    ASSERT_INT_EQUAL(list.freeBlocks, 10, NULL);
    ASSERT_NOT_NULL(list.freeListHeadPtr, NULL);

    BLOCK *current = list.freeListHeadPtr;
    for (int i = 0; i < 10; i++) {
        ASSERT_INT_EQUAL(current->isFree, 1, NULL);
        current = current->next;
    }
    ASSERT_NULL(current, NULL);
}

TEST_CASE(test_allocation) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 10);

    // Allocate blocks
    void *block1 = allocate_block(&list);
    void *block2 = allocate_block(&list);

    // Verify metadata
    ASSERT_INT_EQUAL(list.freeBlocks, 8, NULL);
    ASSERT_NOT_NULL(block1, NULL);
    ASSERT_NOT_NULL(block2, NULL);
    ASSERT_NOT_EQUAL(block1, block2, NULL);

    // Verify blocks are within memory pool
    // assert(block1 >= list.memoryPool && block1 < (void *)((char *)list.memoryPool + list.blockSize * list.totalBlocks));
    // assert(block2 >= list.memoryPool && block2 < (void *)((char *)list.memoryPool + list.blockSize * list.totalBlocks));
}

ADD_TEST(test_seg_list_init);
ADD_TEST(test_allocation);