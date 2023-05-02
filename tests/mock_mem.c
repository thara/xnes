#include "string.h"

#include "mem.h"

#include "mock_mem.h"

static uint8_t memory[10000];

void mem_init() { memset(memory, 0, sizeof(memory)); }

uint8_t MEM_MOCKABLE(mem_read)(struct nes *nes, uint16_t addr) {
  return memory[addr];
}

void MEM_MOCKABLE(mem_write)(struct nes *nes, uint16_t addr, uint8_t val) {
  memory[addr] = val;
}
