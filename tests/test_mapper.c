#include "stdlib.h"

#include "test_common.h"

#include "rom.h"
#include "mapper.h"

TEST(test_detect_mapper) {
  char test_file_path[256];
  get_test_data_path(test_file_path, sizeof(test_file_path), "nestest.nes");

  FILE *file = fopen(test_file_path, "r");
  if (!file) {
    char msg[256];
    snprintf(msg, sizeof(msg), "Failed to open test file: %s\n",
             test_file_path);
    test_precondition_failed(msg);
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

  ROMParseError romError;
  ROM *rom = parse_rom(&rom_file, &romError);
  if (romError != ROM_PARSE_ERROR_NONE) {
    free(buf);
    fclose(file);
    char msg[256];
    snprintf(msg, sizeof(msg), "Failed to parse ROM: %d\n", romError);
    test_precondition_failed(msg);
  }

  MapperError mapperErr;
  Mapper *mapper = detect_mapper(rom, &mapperErr);
  if (mapperErr != MAPPER_ERROR_NONE) {
    rom_release(rom);
    fclose(file);
    char msg[256];
    snprintf(msg, sizeof(msg), "Failed to detect mapper: %d\n", romError);
    test_precondition_failed(msg);
  }

  test_assert_int_eq(0, mapper_no(mapper));
  test_assert_int_eq(MIRRORING_HORIZONTAL, mapper_mirroring(mapper));

  mapper_release(mapper);
  rom_release(rom);
  fclose(file);
}

TEST_SUITE(test_mapper) { RUN_TEST(test_detect_mapper); }

int main(int argc, char *argv[]) {
  RUN_TEST_SUITE(test_mapper);
  TEST_REPORT();
  return TEST_EXIT_CODE;
}
