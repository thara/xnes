#include "mem.h"

uint8_t MEM_MOCKABLE(mem_read)(nes *nes, uint16_t addr) { return 0; }

void MEM_MOCKABLE(mem_write)(nes *nes, uint16_t addr, uint8_t val) {}

#ifdef UNIT_TEST
uint8_t (*mem_read_override)(nes *nes, uint16_t addr) = MEM_MOCKABLE(mem_read);

uint8_t mem_read(nes *nes, uint16_t addr) {
  return mem_read_override(nes, addr);
}

void (*mem_write_override)(nes *nes, uint16_t addr,
                           uint8_t val) = MEM_MOCKABLE(mem_write);
void mem_write(nes *nes, uint16_t addr, uint8_t val) {
  mem_write_override(nes, addr, val);
}
#endif
