#include "ppu.h"

void ppu_ctrl_set(PPU *ppu, PPUCTRL e, bool v) {
  ppu->ctrl &= ~e;
  if (v)
    ppu->ctrl &= e;
}

void ppu_mask_set(PPU *ppu, PPUMASK e, bool v) {
  ppu->mask &= ~e;
  if (v)
    ppu->mask &= e;
}

void ppu_status_set(PPU *ppu, PPUSTATUS e, bool v) {
  ppu->status &= ~e;
  if (v)
    ppu->status &= e;
}

bool ppu_ctrl_enabled(PPU *ppu, PPUCTRL e) { return (ppu->ctrl & e) == e; }

bool ppu_mask_enabled(PPU *ppu, PPUMASK e) { return (ppu->mask & e) == e; }

bool ppu_status_enabled(PPU *ppu, PPUSTATUS e) {
  return (ppu->status & e) == e;
}

uint16_t ppu_coarse_x(uint16_t v) { return (v & 0b000000000011111); }
uint16_t ppu_coarse_y(uint16_t v) { return (v & 0b000001111100000) >> 5; }
uint16_t ppu_nt_select(uint16_t v) { return (v & 0b000110000000000) >> 10; }
uint16_t ppu_fine_y(uint16_t v) { return (v & 0b111000000000000) >> 12; }

void ppu_incr_v(PPU *ppu) {
  if (((ppu->ctrl >> PPUCTRL_VRAM_INCR) & 1) == 1) {
    ppu->v += 32;
  } else {
    ppu->v++;
  }
}
