#include "test_common.h"

#include "stdlib.h"

#include "input.h"

TEST(test_standard_controller_write) {
  StandardController *input = input_standard_controller_new();

  input_write((Input *)input, 0b10101010);
  test_assert_int_eq(input->strobe, false);
  test_assert_bit_eq(input->current, 1);

  input_write((Input *)input, 0b10101011);
  test_assert_int_eq(input->strobe, true);
  test_assert_bit_eq(input->current, 1);

  free(input);
}

TEST(test_standard_controller_read_not_strobe) {
  StandardController *input = input_standard_controller_new();

  input_write((Input *)input, 0b10101010);
  input_standard_controller_update(input, 0b11010101);

  test_assert_byte_eq(0x41, input_read((Input *)input));
  test_assert_bit_eq(0b00000010, input->current);

  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(0b00000100, input->current);

  test_assert_byte_eq(0x41, input_read((Input *)input));
  test_assert_bit_eq(0b00001000, input->current);

  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(0b00010000, input->current);

  test_assert_byte_eq(0x41, input_read((Input *)input));
  test_assert_bit_eq(0b00100000, input->current);

  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(0b01000000, input->current);

  test_assert_byte_eq(0x41, input_read((Input *)input));
  test_assert_bit_eq(0b10000000, input->current);

  test_assert_byte_eq(0x41, input_read((Input *)input));
  test_assert_bit_eq(0, input->current);

  // over reading
  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(0, input->current);
  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(0, input->current);
  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(0, input->current);

  free(input);
}

TEST(test_standard_controller_read_strobe) {
  StandardController *input = input_standard_controller_new();

  input_write((Input *)input, 0b01010101); // set strobe

  input_standard_controller_update(input, 0b10101010);
  test_assert_byte_eq(0x40, input_read((Input *)input));
  test_assert_bit_eq(1, input->current);

  input_standard_controller_update(input, 0b11101011);
  test_assert_byte_eq(0x41, input_read((Input *)input));
  test_assert_bit_eq(1, input->current);

  free(input);
}

TEST_SUITE(test_standard_controller) {
  RUN_TEST(test_standard_controller_write);
  RUN_TEST(test_standard_controller_read_not_strobe);
  RUN_TEST(test_standard_controller_read_strobe);
}

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_standard_controller);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
