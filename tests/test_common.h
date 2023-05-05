#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

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

static void test_int_to_binary(uint16_t n, char* output) {
    int size = sizeof(n) * 8;
    for (int i = size; 0 <= i; i--) {
        output[size - 1 - i] = ((n >>i ) & 1) ? '1' : '0';
    }
    output[size] = '\0';
}

#define test_assert_bit_eq(expect, actual) do {\
  test_assert_count++;\
  int want = (expect);\
  int got = (actual);\
  if (want != got) {\
    char want_str[17];\
    test_int_to_binary(want, want_str);\
    char got_str[17];\
    test_int_to_binary(got, got_str);\
    snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: 0b%s expected but was 0b%s", __func__, __FILE__, __LINE__, want_str, got_str);\
    test_status_failed = true;\
  } else { \
    printf(".");\
  }\
} while(0)

#define test_assert_byte_eq(expect, actual) do {\
  test_assert_count++;\
  int want = (expect);\
  int got = (actual);\
  if (want != got) {\
    snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: 0x%08X expected but was 0x%08X", __func__, __FILE__, __LINE__, want, got);\
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

#define test_precondition_failed(msg) do {\
  test_assert_count++;\
  snprintf(test_last_message, TESTMESSAGE_LEN, "%s failed:\n\t%s:%d: %s", __func__, __FILE__, __LINE__, msg);\
  test_status_failed = true;\
  return;\
} while(0)

#ifndef TEST_DATA_PATH
#define TEST_DATA_PATH ""
#endif

void get_test_data_path(char* buf, size_t len, const char* filename) {
    snprintf(buf, len, "%s/%s", TEST_DATA_PATH, filename);
}

#endif // TEST_RUNNER_H
