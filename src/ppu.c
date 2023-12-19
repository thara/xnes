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

uint16_t ppu_tile_addr(uint16_t v) { return 0x2000 | (v & 0x0FFF); }
uint16_t ppu_attr_addr(uint16_t v) {
  return 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
}

bool ppu_rendering_enabled(PPU *ppu) {
  return ppu_mask_enabled(ppu, PPUMASK_BG) ||
         ppu_mask_enabled(ppu, PPUMASK_SPR);
}

void ppu_incr_v(PPU *ppu) {
  if (((ppu->ctrl >> PPUCTRL_VRAM_INCR) & 1) == 1) {
    ppu->v += 32;
  } else {
    ppu->v++;
  }
}

void ppu_incr_coarse_x(PPU *ppu) {
  // http://wiki.nesdev.com/w/index.php/PPU_scrolling#Coarse_X_increment
  if (ppu_coarse_x(ppu->x) == 31) {
    ppu->v &= !(uint16_t)0x001F; // coarse X = 0
    ppu->v ^= 0x0400;            // switch horizontal nametable
  } else {
    ppu->v++;
  }
}

void ppu_incr_y(PPU *ppu) {
  // http://wiki.nesdev.com/w/index.php/PPU_scrolling#Y_increment
  if (ppu_fine_y(ppu->v) < 7) {
    ppu->v += 0x1000;
  } else {
    ppu->v &= ~(uint16_t)0x7000; // fine Y = 0
    uint16_t y = ppu_coarse_y(ppu->v);
    if (y == 29) {
      y = 0;
      ppu->v ^= 0x0800; // switch vertical nametable
    } else if (y == 31) {
      y = 0;
    } else {
      y++;
    }
    ppu->v = (ppu->v & ~0x03E0) | (y << 5);
  }
}

void ppu_copy_x(PPU *ppu) {
  // http://wiki.nesdev.com/w/index.php/PPU_scrolling#At_dot_257_of_each_scanline
  // v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
  ppu->v = (ppu->v & ~0b10000011111) | (ppu->t & 0b10000011111);
}

void ppu_copy_y(PPU *ppu) {
  // http://wiki.nesdev.com/w/index.php/PPU_scrolling#During_dots_280_to_304_of_the_pre-render_scanline_.28end_of_vblank.29
  // v: IHGF.ED CBA..... = t: IHGF.ED CBA.....
  ppu->v = (ppu->v & ~0b111101111100000) | (ppu->t & 0b111101111100000);
}

void ppu_bg_shift(PPU *ppu) {
  ppu->bg.shift_low <<= 1;
  ppu->bg.attr_shift_high <<= 1;
  ppu->bg.attr_shift_low =
      (ppu->bg.attr_shift_low << 1) | ppu->bg.attr_latch_low;
  ppu->bg.attr_shift_high =
      (ppu->bg.attr_shift_high << 1) | ppu->bg.attr_latch_high;
}

#include <stdio.h>

void ppu_bg_shift_reload(PPU *ppu) {
    if (0 < ppu->bg.high) {
        printf("bsr[%d:%d]%x V:%x\n", ppu->scan.line, ppu->scan.dot, ppu->bg.high, ppu->bg.addr);
    }

  ppu->bg.shift_low = (ppu->bg.shift_low & 0xFF00) | ppu->bg.low;
  ppu->bg.shift_high = (ppu->bg.shift_high & 0xFF00) | ppu->bg.high;
  ppu->bg.attr_latch_low = ppu->bg.at & 1;
  ppu->bg.attr_latch_high = ppu->bg.at & 2;
}

void ppu_sprite_clear(Sprite *spr) {
  spr->enabled = false;
  spr->index = 0xFF;
  spr->x = 0xFF;
  spr->y = 0xFF;
  spr->tile = 0xFF;
  spr->attr = 0xFF;
  spr->low = 0;
  spr->high = 0;
}

uint16_t ppu_sprite_height(PPU *ppu) {
  return ppu_ctrl_enabled(ppu, PPUCTRL_SPR_8x16) ? 16 : 8;
}
