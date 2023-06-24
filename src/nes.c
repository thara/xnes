#include <stdlib.h>

#include "mapper.h"
#include "nes.h"
#include "cpu.h"
#include "cpu_emulation.h"

NES *nes_new() {
  NES *nes = calloc(1, sizeof(NES));
  if (nes == NULL) {
    return NULL;
  }
  nes->interrupt = INTERRUPT_NONE;
  return nes;
}

Cartridge load_cartridge(uint8_t *buf, uint64_t len) {
  Cartridge cart = {0};
  ROM *rom = parse_rom(buf, len, &cart.rom_error);
  if (cart.rom_error != ROM_PARSE_ERROR_NONE) {
    return cart;
  }
  cart.rom = rom;
  cart.mapper = detect_mapper(rom);
  return cart;
}

void nes_insert_cartridge(NES *nes, Mapper *mapper) { nes->mapper = mapper; }

void nes_power_on(NES *nes) {
  // https://wiki.nesdev.com/w/index.php/CPU_power_up_state
  cpu_power_on(nes);

  ppu_power_on(&nes->ppu);
}

void nes_reset(NES *nes) {
  cpu_reset(nes);
  ppu_reset(&nes->ppu);
}

void nes_step(NES *nes) { cpu_step(nes); }

void nes_release(NES *nes) {
  if (nes == NULL) {
    return;
  }
  if (nes->mapper != NULL) {
    mapper_release(nes->mapper);
  }
  free(nes);
}

void nes_tick(NES *nes) {
  // TODO
}
