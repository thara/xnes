#include "ppu.h"

void ppu_power_on(PPU *ppu) {
  // https://www.nesdev.org/wiki/PPU_power_up_state
  ppu_reset(ppu);
  ppu->status = 0;
}

void ppu_reset(PPU *ppu) {
  ppu->ctrl = 0;
  ppu->mask = 0;
  ppu->data = 0;
  ppu->oam_addr = 0;

  ppu->v = 0;
  ppu->t = 0;
  ppu->x = 0;
  ppu->w = false;
}
