#ifndef NES_H
#define NES_H

#include "stdint.h"

#include "cpu.h"
#include "ppu.h"
#include "mapper.h"
#include "rom.h"
#include "input.h"

typedef enum {
  INTERRUPT_NONE,
  INTERRUPT_NMI,
  INTERRUPT_IRQ,
} Interrupt;

typedef struct {
  CPU cpu;
  uint8_t wram[0x0800];

  PPU ppu;

  Interrupt interrupt;

  Mapper *mapper;

  Input *input1;
  Input *input2;
} NES;

NES *nes_new();

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

void nes_release(NES *nes);

void cartridge_release(Cartridge *cart);

void nes_tick(NES *nes);

#endif // NES_H
