#include "stdlib.h"

#include "test_common.h"

#include "rom.h"
#include "mapper.h"

TEST(test_parse_rom) {
  char test_file_path[256];
  get_test_data_path(test_file_path, sizeof(test_file_path), "nestest.nes");

  FILE *file = fopen(test_file_path, "r");
  if (!file) {
    char str[256];
    snprintf(str, sizeof(str), "Failed to open test file: %s\n",
             test_file_path);
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

  ROM rom;
  ROMParseError error;
  parse_rom(&rom_file, &rom, &error);

  test_assert(error == ROM_PARSE_ERROR_NONE, "rom_parse_error should be none");
  test_assert_int_eq(0, rom.mapper_no);
  test_assert_int_eq(1, rom.prg_rom_size);
  test_assert_int_eq(1, rom.chr_rom_size);
  test_assert(!rom.mirroring_vertical, "mirroring should be horizontal");

  free(buf);
  fclose(file);
}

TEST_SUITE(test_rom) { RUN_TEST(test_parse_rom); }

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_rom);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
