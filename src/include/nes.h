#ifndef NES_H
#define NES_H

#include "stdint.h"

#include "cpu.h"
#include "mapper.h"
#include "rom.h"

typedef struct {
  CPU cpu;

  uint8_t wram[0x0800];

  Mapper *mapper;
} NES;

typedef int NESError;

NES *nes_new();

void nes_init(NES *nes, ROMFile *rom_file, NESError *error);

void nes_as_rom_parse_error(NESError nes_error, ROMParseError *error);
void nes_as_mapper_error(NESError nes_error, MapperError *error);

void nes_step(NES *nes);

void nes_release(NES *nes);

void nes_tick(NES *nes);

#endif // NES_H
