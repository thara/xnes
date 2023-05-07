#include "test_common.h"

#include "error.h"

TEST(test_wrap_rom_parse_error) {
  NESError nes_err = wrap_rom_parse_error(ROM_PARSE_ERROR_INVALID_MAGIC_NUMBER);

  ROMParseError rom_err = ROM_PARSE_ERROR_NONE;
  as_rom_parse_error(nes_err, &rom_err);
  test_assert_int_eq(ROM_PARSE_ERROR_INVALID_MAGIC_NUMBER, rom_err);

  MapperError mapper_err = MAPPER_ERROR_NONE;
  as_mapper_error(nes_err, &mapper_err);
  test_assert_int_eq(MAPPER_ERROR_NONE, mapper_err);
}

TEST(test_wrap_mapper_error) {
  NESError nes_err = wrap_mapper_error(MAPPER_ERROR_UNSUPPORTED);

  ROMParseError rom_err = ROM_PARSE_ERROR_NONE;
  as_rom_parse_error(nes_err, &rom_err);
  test_assert_int_eq(ROM_PARSE_ERROR_NONE, rom_err);

  MapperError mapper_err = MAPPER_ERROR_NONE;
  as_mapper_error(nes_err, &mapper_err);
  test_assert_int_eq(MAPPER_ERROR_UNSUPPORTED, mapper_err);
}

TEST_SUITE(test_error) {
  RUN_TEST(test_wrap_rom_parse_error);
  RUN_TEST(test_wrap_mapper_error);
}

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_error);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
