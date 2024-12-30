/* Simple Test Lib */
#ifndef __TEST_H
#define __TEST_H

extern TestFunction test_registry[];
extern const char *test_names[];
extern int test_count;

/* Pointer to test func */
typedef void (*TestFunction)();

/* Test Structure */
typedef struct Test {
    const char *name;
    TestFunction testFunc;
} Test;

#define MAX_TESTS 100
#define TEST_FAIL(line, message) test_fail((line), (message))
#define TEST_CASE(name) void name()

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
#define ASSERT(condition, line, message)     \
    do {                                     \
        if ((condition)) { /* Test passed */ \
        } else {                             \
            TEST_FAIL((line), (message));    \
        }                                    \
    } while (0)

#define ASSERT_NULL(pointer, line, message) ASSERT((pointer == NULL), line, message)
#define ASSERT_NOT_NULL(pointer, line, message) ASSERT((pointer != NULL), line, message)
#define ASSERT_EQUAL(expected, actual, line, message)
#define ASSERT_NOT_EQUAL(expected, actual, line, message)

/* APIS */
void test_fail(int line, const char *message);

#endif