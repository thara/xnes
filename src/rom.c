#include <stdlib.h>

#include "rom.h"

static uint8_t magic_number[4] = {0x4E, 0x45, 0x53, 0x1A};
static uint8_t padding[5];

void parse_rom(uint8_t *buf, uint64_t buf_len, ROM *rom, ROMParseError *error) {
  if (rom == NULL) {
    *error = rom_parse_error_null;
    return;
  }
  rom->raw = buf;

  for (int i = 0; i < 4; i++) {
    if (buf[i] != magic_number[i]) {
      *error = rom_parse_error_invalid_magic_number;
      return;
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
      *error = rom_parse_error_padding;
      return;
    }
  }

  *error = rom_parse_error_none;
}
