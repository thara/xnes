#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <stdint.h>

#include "nes.h"

typedef struct MemoryMap {
  uint8_t (*read)(nes *n, uint16_t addr);
  void (*write)(nes *n, uint16_t addr, uint8_t value);
} memory_map;

void init_memory_map(memory_map mem);

uint8_t mem_read(nes *n, uint16_t addr);

void mem_write(nes *n, uint16_t addr, uint8_t val);

#endif // MEMORY_MAP_H
