#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include "nes.h"

#ifdef UNIT_TEST
  #define MEM_MOCKABLE(func) mockable_##func
#else
  #define MEM_MOCKABLE(func) func
#endif

uint8_t mem_read(struct nes *nes, uint16_t addr);

void mem_write(struct nes *nes, uint16_t addr, uint8_t val);

#ifdef UNIT_TEST
uint8_t MEM_MOCKABLE(mem_read)(struct nes *nes, uint16_t addr);
uint8_t (*mem_read_override)(struct nes *nes, uint16_t addr);

void MEM_MOCKABLE(mem_write)(struct nes *nes, uint16_t addr, uint8_t val);
void (*mem_write_override)(struct nes *nes, uint16_t addr, uint8_t val);
#endif


#endif // MEM_H
