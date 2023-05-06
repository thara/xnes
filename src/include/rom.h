#ifndef ROM_H
#define ROM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t *buf;
  uint64_t len;
} ROMFile;

typedef struct {
  uint8_t mapper_no;
  uint32_t prg_rom_size;
  uint32_t chr_rom_size;
  bool mirroring_vertical;

  uint8_t *raw;
} ROM;

typedef enum {
  ROM_PARSE_ERROR_NONE,
  ROM_PARSE_ERROR_ARG_NULL,
  ROM_PARSE_ERROR_INVALID_MAGIC_NUMBER,
  ROM_PARSE_ERROR_PADDING,
} ROMParseError;

ROM *parse_rom(ROMFile *file, ROMParseError *error);

void rom_release(ROM *rom);

#endif // ROM_H
