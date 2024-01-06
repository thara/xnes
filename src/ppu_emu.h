#ifndef PPU_EMU_H
#define PPU_EMU_H

#include "nes.h"

void ppu_step(NES *nes);

uint8_t ppu_read_register(NES *nes, uint16_t addr);
void ppu_write_register(NES *nes, uint16_t addr, uint8_t value);

uint8_t ppu_read(NES *nes, uint16_t addr);
void ppu_write(NES *nes, uint16_t addr, uint8_t value);

#ifdef UNIT_TEST
uint16_t ppu_coarse_x(uint16_t v);
uint16_t ppu_coarse_y(uint16_t v);
uint16_t ppu_nt_select(uint16_t v);
uint16_t ppu_fine_y(uint16_t v);

uint16_t ppu_tile_addr(uint16_t v);
uint16_t ppu_attr_addr(uint16_t v);

void ppu_status_set(PPU *ppu, PPUSTATUS e, bool v);
bool ppu_ctrl_enabled(PPU *ppu, PPUCTRL c);
bool ppu_mask_enabled(PPU *ppu, PPUMASK m);
bool ppu_status_enabled(PPU *ppu, PPUSTATUS s);
#endif


#endif // PPU_EMU_H
