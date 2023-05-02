#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdbool.h>
#include <stdio.h>

static int test_count = 0;
static int test_assert_count = 0;
static int test_failed_count = 0;
static bool test_status_failed = false;

#define TESTMESSAGE_LEN 1024
static char test_last_message[TESTMESSAGE_LEN];

#define TEST_EXIT_CODE test_failed_count;

#define TEST(func_name) void func_name(void)
#define TEST_SUITE(suite_name) static void suite_name(void)

#define RUN_TEST_SUITE(test_suite) do {\
  test_suite();\
} while(0)

#define RUN_TEST(test) do {\
  test_status_failed = false;\
  test();\
  test_count++;\
  if (test_status_failed) {\
    test_failed_count++;\
    printf("F");\
    printf("\n%s\n", test_last_message);\
  }\
  fflush(stdout);\
} while(0)

#define TEST_REPORT() do {\
  printf("\n\n%d tests, %d assertions, %d failures\n", test_count, test_assert_count, test_failed_count);\
} while(0)

#define test_fail(msg) do {\
  test_assert_count++;\
  snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, msg);\
  test_status_failed = true;\
} while(0)

#define test_assert(test, msg) do {\
  test_assert_count++;\
  if (!(test)) {\
    snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, msg);\
    test_status_failed = true;\
  } else { \
    printf(".");\
  }\
} while(0)

#define test_assert_int_eq(expect, actual) do {\
  test_assert_count++;\
  int want = (expect);\
  int got = (actual);\
  if (want != got) {\
    snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: %d expected but was %d", __func__, __FILE__, __LINE__, want, got);\
    test_status_failed = true;\
  } else { \
    printf(".");\
  }\
} while(0)

#define test_required(test, msg) do {\
  test_assert_count++;\
  if (!(test)) {\
    snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, msg);\
    test_status_failed = true;\
    return;\
  } else { \
    printf(".");\
  }\
} while(0)

#endif // TEST_RUNNER_H
