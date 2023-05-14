#include "stdlib.h"

#include "test_common.h"

#include "nes.h"
#include "rom.h"
#include "cpu_trace.h"
#include "mem.h"

#define nestest_log_assert_byte_eq(expect_trace, actual_trace, field, line_no) \
  do {                                                                         \
    int want = (expect_trace).field;                                           \
    int got = (actual_trace).field;                                            \
    if (want != got) {                                                         \
      char msg[256];                                                           \
      snprintf(msg, sizeof(msg),                                               \
               "%s unmatched: want=%08x, got=%08x line_no=%d\n", "##field",    \
               want, got, line_no);                                            \
      test_fail(msg);                                                          \
      goto NESTEST_END;                                                        \
    }                                                                          \
  } while (0)

#define nestest_log_assert_bit_eq(expect_trace, actual_trace, field, line_no)  \
  do {                                                                         \
    int want = (expect_trace).field;                                           \
    int got = (actual_trace).field;                                            \
    if (want != got) {                                                         \
      char want_str[17];                                                       \
      test_int_to_binary(want, want_str);                                      \
      char got_str[17];                                                        \
      test_int_to_binary(got, got_str);                                        \
      char msg[256];                                                           \
      snprintf(msg, sizeof(msg), "%s unmatched: want=%s, got=%s line_no=%d\n", \
               "##field", want_str, got_str, line_no);                         \
      test_fail(msg);                                                          \
      goto NESTEST_END;                                                        \
    }                                                                          \
  } while (0)

TEST(test_nestest_log) {
  char test_file_path[256];
  get_test_data_path(test_file_path, sizeof(test_file_path), "nestest.nes");

  char log_file_path[256];
  get_test_data_path(log_file_path, sizeof(test_file_path), "nestest.log");

  FILE *file = fopen(test_file_path, "r");
  if (!file) {
    char str[256];
    snprintf(str, sizeof(str), "Failed to open test file: %s\n",
             test_file_path);
    test_precondition_failed(str);
  }

  FILE *log_file = fopen(log_file_path, "r");
  if (!file) {
    fclose(file);
    char str[256];
    snprintf(str, sizeof(str), "Failed to open log file: %s\n", test_file_path);
    test_precondition_failed(str);
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  uint8_t *buf = (uint8_t *)malloc(sizeof(uint8_t) * file_size);
  if (buf == NULL) {
    fclose(file);
    test_precondition_failed("Failed to malloc\n");
  }
  if (fread(buf, 1, file_size, file) != file_size) {
    free(buf);
    fclose(file);
    test_precondition_failed("Failed to read test file\n");
  }

  ROMFile rom_file = {buf, file_size};

  NES *nes = nes_new();

  NESError error = NES_ERROR_NONE;
  nes_init(nes, &rom_file, &error);

  test_assert_int_eq(NES_ERROR_NONE, error);

  nes_power_on(nes);

  // set up initial state for nestest
  nes->cpu.PC = 0xC000;
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state#cite_ref-1
  nes->cpu.P = 0x24;
  nes->cpu.cycles = 7;

  int line_no;
  char line[256];
  while (fgets(line, sizeof(line), file)) {
    CPUTrace actual = cpu_trace(nes);

    nes_step(nes);

    CPUTrace expected;
    if (!parse_cpu_trace(line, &expected)) {
      // fail
      char msg[256];
      snprintf(msg, sizeof(msg), "Failed to parse nestest.log: line_no=%d\n",
               line_no);
      test_fail(msg);
      goto NESTEST_END;
    }

    nestest_log_assert_byte_eq(expected, actual, current_state.PC, line_no);
    nestest_log_assert_byte_eq(expected, actual, next_opcode, line_no);
    nestest_log_assert_byte_eq(expected, actual, next_operand1, line_no);
    nestest_log_assert_byte_eq(expected, actual, next_operand2, line_no);

    nestest_log_assert_byte_eq(expected, actual, next_instruction.mnemonic,
                               line_no);
    nestest_log_assert_byte_eq(expected, actual, next_instruction.mode,
                               line_no);

    nestest_log_assert_byte_eq(expected, actual, current_state.A, line_no);
    nestest_log_assert_byte_eq(expected, actual, current_state.X, line_no);
    nestest_log_assert_byte_eq(expected, actual, current_state.Y, line_no);
    nestest_log_assert_byte_eq(expected, actual, current_state.P, line_no);
    nestest_log_assert_byte_eq(expected, actual, current_state.S, line_no);

    nestest_log_assert_byte_eq(expected, actual, current_state.cycles, line_no);

    line_no++;
  }

  test_assert_int_eq(26560, nes->cpu.cycles);

  test_assert_int_eq(0, mem_read(nes, 0x0002));
  test_assert_int_eq(0, mem_read(nes, 0x0003));

NESTEST_END:
  nes_release(nes);
  fclose(log_file);
  fclose(file);
}

TEST_SUITE(test_nestest) { RUN_TEST(test_nestest_log); }

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_nestest);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
