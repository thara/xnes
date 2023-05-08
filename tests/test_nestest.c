#include "stdlib.h"

#include "test_common.h"

#include "nes.h"
#include "rom.h"

TEST(test_nestest_log) {
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

  NES *nes = nes_new();

  NESError error = NES_ERROR_NONE;
  nes_init(nes, &rom_file, &error);

  test_assert_int_eq(NES_ERROR_NONE, error);

  //TODO
}

TEST_SUITE(test_nestest) { RUN_TEST(test_nestest_log); }

int main(int argc, char *argv[]) {
  /* RUN_TEST_SUITE(test_nestest); */
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
