#include "mem.h"

#include "mapper.h"

uint8_t MEM_MOCKABLE(mem_read)(NES *nes, uint16_t addr) {
  if (addr <= 0x1FFF) {
    return nes->wram[addr % 0x0800];
  } else if (0x4020 <= addr && addr <= 0xFFFF) {
    return mapper_read(nes->mapper, addr);
  }
  // TODO PPU, APU, controller
  return 0;
}

void MEM_MOCKABLE(mem_write)(NES *nes, uint16_t addr, uint8_t val) {
  // OAMDMA
  // https://wiki.nesdev.org/w/index.php?title=PPU_registers#OAM_DMA_.28.244014.29_.3E_write
  if (addr == 0x4014) {
    uint16_t start = (uint16_t)val + 0x100;
    for (uint16_t i = 0; i < 0xFF; i++) {
      uint8_t m = mem_read(nes, start + i);
      mem_write(nes, 0x2004, m);
    }
    nes_tick(nes);

    if (nes->cpu.cycles % 2 == 1) {
      nes_tick(nes);
    }
    return;
  }

  if (addr <= 0x1FFF) {
    nes->wram[addr % 0x0800] = val;
  } else if (0x4020 <= addr && addr <= 0xFFFF) {
    mapper_write(nes->mapper, addr, val);
  }
  // TODO PPU, APU, controller
}

#ifdef UNIT_TEST
uint8_t (*mem_read_override)(NES *nes, uint16_t addr) = MEM_MOCKABLE(mem_read);

uint8_t mem_read(NES *nes, uint16_t addr) {
  return mem_read_override(nes, addr);
}

void (*mem_write_override)(NES *nes, uint16_t addr,
                           uint8_t val) = MEM_MOCKABLE(mem_write);
void mem_write(NES *nes, uint16_t addr, uint8_t val) {
  mem_write_override(nes, addr, val);
}
#endif
