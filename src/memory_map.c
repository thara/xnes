#include "memory_map.h"

memory_map current;

void init_memory_map(memory_map mem) { current = mem; }

uint8_t mem_read(nes *n, uint16_t addr) { return current.read(n, addr); }

void mem_write(nes *n, uint16_t addr, uint8_t val) {
  current.write(n, addr, val);
}
