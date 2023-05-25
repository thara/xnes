#include "ppu.h"

void ppu_ctrl_set(PPU *ppu, PPUCTRL e, bool v) {
  ppu->ctrl &= ~(1 << e);
  ppu->ctrl |= v << e;
}

void ppu_mask_set(PPU *ppu, PPUMASK e, bool v) {
  ppu->mask &= ~(1 << e);
  ppu->mask |= v << e;
}

void ppu_status_set(PPU *ppu, PPUSTATUS e, bool v) {
  ppu->status &= ~(1 << e);
  ppu->status |= v << e;
}

void ppu_incr_v(PPU *ppu) {
  if (((ppu->ctrl >> PPUCTRL_VRAM_INCR) & 1) == 1) {
    ppu->v += 32;
  } else {
    ppu->v++;
  }
}
