/* Simple Test Lib */
#ifndef __TEST_H
#define __TEST_H

#include <stdbool.h>
#include <stdio.h>

/* Pointer to test func */
typedef void (*TestFunction)();

/* Test Structure */
typedef struct Test {
    const char *name;
    TestFunction testFunc;
} Test;

/* Running test context */
typedef struct TEST {
    const char *currentTestName;
    const char *testFile;
    bool currentTestFailed;
    int numTestsPassed;
    int numTestsFailed;
} TEST;

extern TestFunction test_registry[];
extern const char *test_names[];
extern int test_count;

#define MAX_TESTS 100
#define TEST_FAIL(line, file, message) test_fail((line), (file), (message))
#define TEST_CASE(name) void name()
#define OUTPUT_FLUSH() (void)fflush(stdout)
#define ABORT_TEST() return

/* Register test function
 * void name(); forward declaration
 * void register_##name() register function
 */
#define ADD_TEST(name)                                    \
    void name();                                          \
    __attribute__((constructor)) void register_##name() { \
        test_registry[test_count] = name;                 \
        test_names[test_count] = #name;                   \
        test_count++;                                     \
    }                                                     \
    void name()

/* ===== Test Asserts ===== */
#define ASSERT(condition, file, line, message)    \
    do {                                          \
        if ((condition)) { /* Test passed */      \
        } else {                                  \
            TEST_FAIL((line), (file), (message)); \
        }                                         \
    } while (0)

#define ASSERT_NULL(pointer, message) ASSERT((pointer == NULL), __FILE__, __LINE__, message)
#define ASSERT_NOT_NULL(pointer, message) ASSERT((pointer != NULL), __FILE__, __LINE__, message)
#define ASSERT_EQUAL(expected, actual, message) ASSERT(((expected) == (actual)), __FILE__, __LINE__, message)
#define ASSERT_NOT_EQUAL(expected, actual, message) ASSERT(((expected) != (actual)), __FILE__, __LINE__, message)
#define ASSERT_INT_EQUAL(expected, actual, message) assert_equal_number((expected), (actual), __FILE__, __LINE__, (message))
#define ASSERT_TRUE(condition, message) ASSERT((condition), __FILE__, __LINE__, (message))

/* APIS */
void test_fail(int line, const char *file, const char *message);
void printTestResult(const char *file, const int line);
void assert_equal_number(const int expected, const int actual, const char *file, const int line, const char *msg);

#endif