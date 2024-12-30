#include <stdio.h>
#include <stdlib.h>

#include "test.h"

TestFunction test_registry[MAX_TESTS];
const char *test_names[MAX_TESTS];
int test_count = 0;
TEST testDetail;

/* */
#define TEST_BAIL()                       \
    do {                                  \
        testDetail.currentTestFailed = 1; \
        OUTPUT_FLUSH();                   \
        ABORT_TEST();                     \
    } while (0)

const char *TestStrPass = "\x1B[1;32mPASS\x1B[0m";
const char *TestStrFail = "\x1B[1;31mFAIL\x1B[0m";

const char *TestStrExpected = " Expected: ";
const char *TestStrActual = " Received: ";

/* Print test results format */
void printTestResult(const char *file, const int line) {
    printf("%s:%d: %s: ", file, line, testDetail.currentTestName);
}

/* Print passing test results format */
void printTestStatus(const char *status) {
    printf(" %s: ", status);
}

/* Test int number */
void assert_equal_number(const int expected, const int actual, const int line, const char *file, const char *msg) {
    if (expected != actual) {
        printTestResult(file, line);
        printTestStatus(TestStrFail);
        printf("%s", TestStrExpected);
        printf("%d", expected);
        printf("%s", TestStrActual);
        printf("%d", actual);
        printf("\n");
        TEST_BAIL();
    }
}

/* Failing test */
void test_fail(const int line, const char *file, const char *msg) {
    printTestResult(file, line);
    printTestStatus(TestStrFail);
    TEST_BAIL();
}

void finish_test() {
    if (!testDetail.currentTestFailed)
        printf("%s: %s\n", testDetail.currentTestName, TestStrPass);
    else
        testDetail.numTestsFailed++;

    testDetail.currentTestFailed = 0;
}

/* Test Runner */
void test_run(TestFunction testFunc, const char *testFuncName) {
    /* Set current test details for reporting */
    testDetail.currentTestName = testFuncName;
    testFunc();
    /* Test is done at this point */
    finish_test();
}

int main(void) {
    int i;
    printf("Number of tests discovered: %d\n", test_count);
    /* Reset */
    testDetail.currentTestFailed = 0;
    testDetail.currentTestName = NULL;
    testDetail.numTestsPassed = 0;
    testDetail.numTestsFailed = 0;

    /* Run tests */
    for (i = 0; i < test_count; i++) {
        test_run(test_registry[i], test_names[i]);
    }

    return 0;
}