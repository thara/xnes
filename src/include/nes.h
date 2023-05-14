#ifndef NES_H
#define NES_H

#include "stdint.h"

#include "cpu.h"
#include "error.h"
#include "mapper.h"
#include "rom.h"
#include "input.h"

typedef struct {
  CPU cpu;

  uint8_t wram[0x0800];

  Mapper *mapper;

  Input* input1;
  Input* input2;
} NES;

NES *nes_new();

void nes_init(NES *nes, ROMFile *rom_file, NESError *error);

void nes_step(NES *nes);

void nes_release(NES *nes);

void nes_tick(NES *nes);

#endif // NES_H
