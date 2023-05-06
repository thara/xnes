#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include "nes.h"

#ifdef UNIT_TEST
#define MEM_MOCKABLE(func) mockable_##func
#else
#define MEM_MOCKABLE(func) func
#endif

uint8_t mem_read(NES *nes, uint16_t addr);

void mem_write(NES *nes, uint16_t addr, uint8_t val);

#ifdef UNIT_TEST
uint8_t MEM_MOCKABLE(mem_read)(NES *nes, uint16_t addr);
uint8_t (*mem_read_override)(NES *nes, uint16_t addr);

void MEM_MOCKABLE(mem_write)(NES *nes, uint16_t addr, uint8_t val);
void (*mem_write_override)(NES *nes, uint16_t addr, uint8_t val);
#endif

#endif // MEM_H
