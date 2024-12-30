#include "mem.h"
#include "test.h"

TEST_CASE(test_seg_list_init) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 10);

    // TODO: message field optional
    /* Check segregated list properties */
    ASSERT_INT_EQUAL(list.blockSize, 4096, NULL);
    ASSERT_INT_EQUAL(list.totalBlocks, 10, NULL);
    ASSERT_INT_EQUAL(list.freeBlocks, 10, NULL);
    ASSERT_NOT_NULL(list.freeListHeadPtr, NULL);

    /* Check block initialization */
    BLOCK *current = list.freeListHeadPtr;
    for (int i = 0; i < 10; i++) {
        ASSERT_INT_EQUAL(current->isFree, 1, NULL);
        current = current->next;
    }
    /* Check last node in list */
    ASSERT_NULL(current, NULL);
}

TEST_CASE(test_allocation) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 10);

    /* Check Block allocation */
    void *block1 = allocate_block(&list);
    void *block2 = allocate_block(&list);

    /* Check block meta data */
    ASSERT_INT_EQUAL(list.freeBlocks, 8, NULL);
    ASSERT_NOT_NULL(block1, NULL);
    ASSERT_NOT_NULL(block2, NULL);
    ASSERT_NOT_EQUAL(block1, block2, NULL);

    /* Check blocks are within memory pool */
    ASSERT_TRUE((block1 >= list.memoryPoolPtr && block1 < (void *)(char *)list.memoryPoolPtr + list.blockSize * list.totalBlocks), NULL);
    ASSERT_TRUE((block2 >= list.memoryPoolPtr && block2 < (void *)(char *)list.memoryPoolPtr + list.blockSize * list.totalBlocks), NULL);
}

TEST_CASE(test_deallocate_mem_pool) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 10);

    /* deallocate */
    deallocate_mem_pool(&list);

    /* Check metadata */
    ASSERT_EQUAL(list.freeListHeadPtr, NULL, NULL);
    ASSERT_EQUAL(list.memoryPoolPtr, NULL, NULL);
    ASSERT_INT_EQUAL(list.freeBlocks, 0, NULL);
    ASSERT_INT_EQUAL(list.totalBlocks, 0, NULL);
}

TEST_CASE(test_deallocate_block) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 10);

    /* Check metadata */
    BLOCK *block = allocate_block(&list);
    ASSERT_NOT_NULL(block, NULL);
    ASSERT_TRUE(block->isFree == 0, NULL);

    deallocate_block(&list, block);
    ASSERT_FALSE(block->isFree == 0, NULL);
}

TEST_CASE(test_extend_mem_pool) {
    SEGREGATED_LIST list;
    seg_list_init(&list, 4096, 2);

    /* Check metadata */
    ASSERT_EQUAL(list.totalBlocks, 2, NULL);
    ASSERT_EQUAL(list.freeBlocks, 2, NULL);
    ASSERT_NOT_NULL(list.freeListHeadPtr, NULL);
    ASSERT_NOT_NULL(list.memoryPoolPtr, NULL);

    extend_mem_pool(&list);
    /* Check metadata (pooled doubled) */
    ASSERT_EQUAL(list.totalBlocks, 4, NULL);
    ASSERT_EQUAL(list.freeBlocks, 4, NULL);
    ASSERT_NOT_NULL(list.freeListHeadPtr, NULL);
    ASSERT_NOT_NULL(list.memoryPoolPtr, NULL);
}

ADD_TEST(test_seg_list_init);
ADD_TEST(test_allocation);
ADD_TEST(test_deallocate_mem_pool);
ADD_TEST(test_deallocate_block);
ADD_TEST(test_extend_mem_pool);