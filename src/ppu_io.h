#ifndef PPU_IO_H
#define PPU_IO_H

#include "nes.h"

uint8_t ppu_read_register(NES *nes, uint16_t addr);
void ppu_write_register(NES *nes, uint16_t addr, uint8_t value);

uint8_t ppu_read(NES *nes, uint16_t addr);
void ppu_write(NES *nes, uint16_t addr, uint8_t value);

#endif // PPU_IO_H
