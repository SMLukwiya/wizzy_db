#include <stdio.h>
#include <stdlib.h>

#include "test.h"

TestFunction test_registry[MAX_TESTS];
const char *test_names[MAX_TESTS];
int test_count = 0;

const char *TestStrPass = "\x1B[42mPASS\x1B[0m";
const char *TestStrFail = "\x1B[41mFAIL\x1B[0m";

int main(void) {
    int i;
    printf("Number of tests discovered: %d\n", test_count);
    /* Run tests */
    for (i = 0; i < test_count; i++) {
        printf("Test: %s\n", test_names[i]);
        test_registry[i]();
    }

    return 0;
}