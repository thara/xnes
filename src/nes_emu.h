#ifndef NES_EMU_H
#define NES_EMU_H

#include "nes.h"
#include "rom.h"

typedef struct {
  ROM *rom;
  ROMParseError rom_error;

  Mapper *mapper;
} Cartridge;

Cartridge *load_cartridge(uint8_t *buf, uint64_t len);

void nes_insert_cartridge(NES *nes, Mapper* mapper);

void nes_power_on(NES *nes);

void nes_reset(NES *nes);

void nes_step(NES *nes);

void cartridge_release(Cartridge *cart);

void nes_tick(NES *nes);

#endif // NES_EMU_H
