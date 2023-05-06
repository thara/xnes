#include <stdlib.h>

#include "rom.h"

static uint8_t magic_number[4] = {0x4E, 0x45, 0x53, 0x1A};
static uint8_t padding[5];

ROM *parse_rom(ROMFile *file, ROMParseError *error) {
  ROM *rom = malloc(sizeof(ROM));
  if (rom == NULL) {
    *error = ROM_PARSE_ERROR_CAN_NOT_ALLOCATED;
    return NULL;
  }
  uint8_t *buf = file->buf;
  rom->raw = buf;

  for (int i = 0; i < 4; i++) {
    if (buf[i] != magic_number[i]) {
      *error = ROM_PARSE_ERROR_INVALID_MAGIC_NUMBER;
      return NULL;
    }
  }
  buf += 4;

  rom->prg_rom_size = *(buf++);
  rom->chr_rom_size = *(buf++);

  uint8_t flag6 = *(buf++);
  uint8_t flag7 = *(buf++);

  rom->mapper_no = (flag6 & 0b1111000) | (flag7 & 0b11110000) << 4;
  rom->mirroring_vertical = (flag6 & 1) == 1;

  // skip flag 8, 9, 10
  buf += 3;

  // validate unused padding
  for (int i = 0; i < 5; i++) {
    if (buf[i] != padding[i]) {
      *error = ROM_PARSE_ERROR_PADDING;
      return NULL;
    }
  }

  *error = ROM_PARSE_ERROR_NONE;
  return rom;
}

void rom_release(ROM *rom) {
  if (rom == NULL) {
    return;
  }

  free(rom->raw);
  free(rom);
}
