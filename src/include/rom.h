#ifndef ROM_H
#define ROM_H

#include <stdbool.h>
#include <stdint.h>

struct rom {
   uint8_t mapper_no;
   uint32_t prg_rom_size;
   uint32_t chr_rom_size;
   bool mirroring_vertical;

   uint8_t* raw;
};

enum rom_parse_error {
    rom_parse_error_none,
    rom_parse_error_null,
    rom_parse_error_invalid_magic_number,
    rom_parse_error_padding,
};

void parse_rom(uint8_t* buf, uint64_t buf_len, struct rom* rom, enum rom_parse_error* error);

#endif // ROM_H
