#include "ppu_emu.h"

#include "ppu.h"

#include "stdio.h"

#define TILE_HEIGHT 8

// Notes in https://www.nesdev.org/w/images/default/d/d1/Ntsc_timing.png
#define PIXEL_DELAYED 2

void ppu_status_set(PPU *ppu, PPUSTATUS e, bool v) {
  ppu->status &= ~e;
  if (v)
    ppu->status |= e;
}

bool ppu_ctrl_enabled(PPU *ppu, PPUCTRL e) { return (ppu->ctrl & e) == e; }

bool ppu_mask_enabled(PPU *ppu, PPUMASK e) { return (ppu->mask & e) == e; }

bool ppu_status_enabled(PPU *ppu, PPUSTATUS e) {
  return (ppu->status & e) == e;
}

bool ppu_rendering_enabled(PPU *ppu) {
  return ppu_mask_enabled(ppu, PPUMASK_BG) ||
         ppu_mask_enabled(ppu, PPUMASK_SPR);
}

/// https://wiki.nesdev.com/w/index.php/PPU_scrolling#PPU_internal_registers
///
/// yyy NN YYYYY XXXXX
/// ||| || ||||| +++++-- coarse X scroll
/// ||| || +++++-------- coarse Y scroll
/// ||| ++-------------- nametable select
/// +++----------------- fine Y scroll

uint16_t ppu_coarse_x(uint16_t v) { return (v & 0b000000000011111); }
uint16_t ppu_coarse_y(uint16_t v) { return (v & 0b000001111100000) >> 5; }
uint16_t ppu_nt_select(uint16_t v) { return (v & 0b000110000000000) >> 10; }
uint16_t ppu_fine_y(uint16_t v) { return (v & 0b111000000000000) >> 12; }

// https://www.nesdev.org/wiki/PPU_scrolling#Tile_and_attribute_fetching
//
// NN 1111 YYY XXX
// || |||| ||| +++-- high 3 bits of coarse X (x/4)
// || |||| +++------ high 3 bits of coarse Y (y/4)
// || ++++---------- attribute offset (960 bytes)
// ++--------------- nametable select
uint16_t ppu_tile_addr(uint16_t v) { return 0x2000 | (v & 0x0FFF); }
uint16_t ppu_attr_addr(uint16_t v) {
  return 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);
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
  if (ppu_coarse_x(ppu->v) == 31) {
    ppu->v &= ~(uint16_t)0x001F; // coarse X = 0
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

void render_pixel(NES *nes);

void ppu_step(NES *nes) {
  PPU *ppu = &nes->ppu;
  bool pre = ppu->scan.line == 261;

  if (ppu->scan.line == 261 || ppu->scan.line <= 239) {
    // pre-render or visible

    // sprites
    switch (ppu->scan.dot) {
    case 1:
      // clear OAM
      for (int i = 0; i < 8; i++) {
        ppu_sprite_clear(&ppu->spr.secondary[i]);
      }
      if (pre) {
        ppu_status_set(ppu, PPUSTATUS_SPR_OVERFLOW, false);
        ppu_status_set(ppu, PPUSTATUS_SPR_0_HIT, false);
      }
      break;

    case 257:
      // eval sprites
      {
        uint8_t n = 0;
        for (int i = 0; i < SPRITE_COUNT; i++) {
          uint8_t y = ppu->spr.oam[i * 4];
          uint16_t row = pre ? (uint16_t)y - 1 : ppu->scan.dot - (uint16_t)y;
          if (ppu_sprite_height(ppu) <= row) {
            continue;
          }
          ppu->spr.secondary[n].enabled = true;
          ppu->spr.secondary[n].index = (uint8_t)i;
          ppu->spr.secondary[n].y = y;
          ppu->spr.secondary[n].tile = ppu->spr.oam[i * 4 + 1];
          ppu->spr.secondary[n].attr = ppu->spr.oam[i * 4 + 2];
          ppu->spr.secondary[n].x = ppu->spr.oam[i * 4 + 3];
          n++;
          if (SPRITE_LIMIT <= n) {
            ppu_status_set(ppu, PPUSTATUS_SPR_OVERFLOW, true);
            break;
          }
        }
        break;
      }

    case 321:
      // load sprites
      {
        for (int i = 0; i < SPRITE_LIMIT; i++) {
          ppu->spr.primary[i] = ppu->spr.secondary[i];
          Sprite *spr = &ppu->spr.primary[i];

          uint16_t tile = (uint16_t)spr->tile;

          uint16_t base;
          if (ppu_ctrl_enabled(ppu, PPUCTRL_SPR_8x16)) {
            tile &= 0xFE;
            base = (tile & 1) * 0x1000;
          } else {
            base = ppu_ctrl_enabled(ppu, PPUCTRL_SPR_TABLE) ? 0x1000 : 0x0000;
          }

          uint16_t y = (ppu->scan.line - spr->y) % ppu_sprite_height(ppu);
          if (spr->attr & SPRITE_ATTR_FLIP_HORIZONTALLY) {
            y ^= ppu_sprite_height(ppu) - 1; // vertical flip
          }

          uint16_t addr = base + tile * 16;
          addr += y + (y & 8); // second tile on 8x16

          ppu->spr.primary[i].low = ppu_read(nes, addr);
          ppu->spr.primary[i].high = ppu_read(nes, addr + 8);
        }
      }
      break;
    }

    // background
    if ((2 <= ppu->scan.dot && ppu->scan.dot <= 255) ||
        (322 <= ppu->scan.dot && ppu->scan.dot <= 337)) {
      render_pixel(nes);

      // https://wiki.nesdev.org/w/index.php/PPU_scrolling#Tile_and_attribute_fetching
      switch (ppu->scan.dot % 8) {
      case 1: // name table addr
        ppu->bg.addr = ppu_tile_addr(ppu->v);
        ppu_bg_shift_reload(&nes->ppu);
        break;
      case 2: // name table fetch
        ppu->bg.nt = ppu_read(nes, ppu->bg.addr);
        break;
      case 3: // attribute addr
        ppu->bg.addr = ppu_attr_addr(ppu->v);
        break;
      case 4: // attribute fetch
        ppu->bg.at = ppu_read(nes, ppu->bg.addr);
        if (0 < (ppu_coarse_y(ppu->v) & 0b10)) {
          ppu->bg.at >>= 4;
        }
        if (0 < (ppu_coarse_x(ppu->v) & 0b10)) {
          ppu->bg.at >>= 2;
        }
        break;
      case 5: // bg low addr
      {
        uint16_t base =
            ppu_ctrl_enabled(&nes->ppu, PPUCTRL_BG_TABLE) ? 0x1000 : 0;
        uint16_t index = ((uint16_t)ppu->bg.nt) * TILE_HEIGHT * 2;
        ppu->bg.addr = base + index + ppu_fine_y(ppu->v);
        break;
      }
      case 6: // bg low fetch
        ppu->bg.low = (uint16_t)ppu_read(nes, ppu->bg.addr);
        break;
      case 7: // bg high addr
        ppu->bg.addr += TILE_HEIGHT;
        break;
      case 0: // bg high fetch
        ppu->bg.high = (uint16_t)ppu_read(nes, ppu->bg.addr);
        if (ppu_rendering_enabled(&nes->ppu)) {
          ppu_incr_coarse_x(&nes->ppu);
        }
        break;
      }
    } else if (ppu->scan.dot == 256) {
      render_pixel(nes);

      ppu->bg.high = (uint16_t)ppu_read(nes, ppu->bg.addr);
      if (ppu_rendering_enabled(&nes->ppu)) {
        ppu_incr_y(&nes->ppu);
      }
    } else if (ppu->scan.dot == 257) {
      render_pixel(nes);

      ppu_bg_shift_reload(&nes->ppu);
      if (ppu_rendering_enabled(&nes->ppu)) {
        ppu_copy_x(&nes->ppu);
      }
    } else if (280 <= ppu->scan.dot && ppu->scan.dot <= 304 && pre) {
      if (ppu_rendering_enabled(&nes->ppu)) {
        ppu_copy_y(&nes->ppu);
      }

    } else if (ppu->scan.dot == 1) {
      // no shift reloading
      ppu->bg.addr = ppu_tile_addr(ppu->v);
      if (pre)
        ppu_status_set(&nes->ppu, PPUSTATUS_VBLANK, false);

    } else if (ppu->scan.dot == 321 || ppu->scan.dot == 339) {
      // no shift reloading(2)
      ppu->bg.addr = ppu_tile_addr(ppu->v);

    } else if (ppu->scan.dot == 338) {
      // unused name table fetches
      ppu->bg.nt = ppu_read(nes, ppu->bg.addr);

    } else if (ppu->scan.dot == 340) {
      // unused name table fetches(2)
      ppu->bg.nt = ppu_read(nes, ppu->bg.addr);
      if (pre && ppu_rendering_enabled(&nes->ppu) && ppu->frames % 2 != 0) {
        ppu->scan.dot++; // skip 0 cycle on visible frame
      }
    }

  } else if (ppu->scan.line == 240 && ppu->scan.dot == 1) {
    // post-render
    nes->renderer.update_frame(nes->renderer.state, ppu->buffer,
                               SCREEN_WIDTH * SCREEN_HEIGHT);
  } else if (ppu->scan.line == 241 && ppu->scan.dot == 1) {
    // NMI
    ppu_status_set(&nes->ppu, PPUSTATUS_VBLANK, true);
    if (ppu_ctrl_enabled(&nes->ppu, PPUCTRL_NMI)) {
      nes->interrupt = INTERRUPT_NMI;
    }
  }

  ppu->scan.dot++;
  if (340 < ppu->scan.dot) {
    ppu->scan.dot %= 341;
    ppu->scan.line++;
    if (261 < ppu->scan.line) {
      ppu->scan.line = 0;
      ppu->frames++;
    }
  }
}

#define NTH(b, n) (((b) >> (n)) & 1)

void render_pixel(NES *nes) {
  PPU *ppu = &nes->ppu;
  uint16_t x = ppu->scan.dot - PIXEL_DELAYED;

  // visible
  if (ppu->scan.line < 240 && x < 256) {
    // background
    uint8_t bg = 0;
    if (ppu_mask_enabled(&nes->ppu, PPUMASK_BG) &&
        !(!ppu_mask_enabled(&nes->ppu, PPUMASK_BG_LEFT) && x < 8)) {
      bg = NTH(ppu->bg.shift_high, 15 - ppu->x) << 1 |
           NTH(ppu->bg.shift_low, 15 - ppu->x);
      if (0 < bg) {
        bg |= (NTH(ppu->bg.attr_shift_high, 7 - ppu->x) << 1 |
               NTH(ppu->bg.attr_shift_low, 7 - ppu->x))
              << 2;
      }
    }
    // sprites
    uint8_t spr = 0;
    bool behind_bg = 0;
    if (ppu_mask_enabled(&nes->ppu, PPUMASK_SPR) &&
        !(!ppu_mask_enabled(&nes->ppu, PPUMASK_SPR_LEFT) && x < 8)) {
      // https://www.nesdev.org/wiki/PPU_sprite_priority
      // Sprites with lower OAM indices are drawn in front
      for (int i = 7; 0 <= i; i--) {
        Sprite s = ppu->spr.primary[i];
        if (!s.enabled) {
          continue;
        }
        uint16_t spr_x = x - s.x;
        if (8 <= spr_x) {
          continue;
        }
        if (0 < (s.attr & SPRITE_ATTR_FLIP_HORIZONTALLY)) {
          spr_x ^= 7; // horizontal flip
        }
        uint8_t px = 7 - spr_x;
        uint8_t palette = ((s.high >> px) & 1) << 1 | (s.low >> px) << 1;
        if (palette == 0) {
          continue;
        }
        if (i == 0 && bg != 0 && x != 255) {
          ppu_status_set(ppu, PPUSTATUS_SPR_0_HIT, true);
        }
        spr = (palette | (s.attr & SPRITE_ATTR_PALETTE) << 2) + 0x10;
        behind_bg = 0 < (s.attr & SPRITE_ATTR_BEHIND_BG);
      }
    }

    uint8_t color = 0;
    if (ppu_rendering_enabled(&nes->ppu)) {
      if (bg == 0 && spr == 0) {
        // default
      } else if (bg == 0 && 0 < spr) {
        color = spr;
      } else if (0 < bg && spr == 0) {
        color = bg;
      } else if (0 < bg && 0 < spr) {
        color = behind_bg ? bg : spr;
      }
    }
    ppu->buffer[ppu->scan.line * 256 + x] = ppu_read(nes, 0x3F00 + color);
  }

  ppu_bg_shift(&nes->ppu);
}

uint8_t ppu_read_register(NES *nes, uint16_t addr) {
  uint8_t ret;

  switch (addr) {
  case 0x2002:
    // PPUSTATUS
    ret = nes->ppu.status | (nes->ppu.cpu_data_bus & 0b11111);
    ppu_status_set(&nes->ppu, PPUSTATUS_VBLANK, false);
    nes->ppu.w = false;
    // race condition
    if (nes->ppu.scan.line == 241 && nes->ppu.scan.dot < 2) {
      ret &= ~0x80;
    }
    break;
  case 0x2004:
    // OAMDATA
    // https://wiki.nesdev.com/w/index.php_sprite_evaluation
    if (nes->ppu.scan.line < 240 && 1 <= nes->ppu.scan.dot &&
        nes->ppu.scan.dot <= 64) {
      // during sprite evaluation
      ret = 0xFF;
    } else {
      ret = nes->ppu.spr.oam[nes->ppu.oam_addr];
    }
    break;
  case 0x2007:
    // PPUDATA
    if (nes->ppu.v <= 0x3EFF) {
      ret = nes->ppu.data;
      nes->ppu.data = ppu_read(nes, nes->ppu.v);
    } else {
      ret = ppu_read(nes, nes->ppu.v);
    }
    ppu_incr_v(&nes->ppu);
    break;
  default:
    ret = nes->ppu.cpu_data_bus;
  }
  nes->ppu.cpu_data_bus = ret;
  return ret;
}

void ppu_write_register(NES *nes, uint16_t addr, uint8_t value) {
  switch (addr) {
  case 0x2000: // PPUCTRL
    nes->ppu.ctrl = value;
    // t: ...BA.. ........ = d: ......BA
    nes->ppu.t = (nes->ppu.t & ~0b110000000000) |
                 ((uint16_t)(nes->ppu.ctrl & PPUCTRL_NT) << 10);
    break;
  case 0x2001: // PPUMASK
    nes->ppu.mask = value;
    break;
  case 0x2003: // OAMADDR
    nes->ppu.oam_addr = value;
    break;
  case 0x2004: // OAMDATA
    nes->ppu.spr.oam[nes->ppu.oam_addr] = value;
    nes->ppu.oam_addr++;
    break;
  case 0x2005: // PPUSCROLL
  {
    // http://wiki.nesdev.org/w/index.php_scrolling#.242005_first_write_.28w_is_0.29
    // http://wiki.nesdev.org/w/index.php_scrolling#.242005_second_write_.28w_is_1.29
    uint16_t d = value;
    if (!nes->ppu.w) {
      // first write
      // t: ....... ...HGFED = d: HGFED...
      // x:              CBA = d: .....CBA
      nes->ppu.t = (nes->ppu.t & ~0b11111) | (d & 0b11111000) >> 3;
      nes->ppu.x = value & 0b111;
    } else {
      // second write
      // t: CBA..HG FED..... = d: HGFEDCBA
      nes->ppu.t = (nes->ppu.t & ~0b111001111100000) | ((d & 0b111) << 12) |
                   ((d & 0b11111000) << 2);
    }
    nes->ppu.w = !nes->ppu.w;
    break;
  }
  case 0x2006: // PPUADDR
  {
    // http://wiki.nesdev.org/w/index.php_scrolling#.242006_first_write_.28w_is_0.29
    // http://wiki.nesdev.org/w/index.php_scrolling#.242006_second_write_.28w_is_1.29
    uint16_t d = value;
    if (!nes->ppu.w) {
      // first write
      // t: .FEDCBA ........ = d: ..FEDCBA
      // t: X...... ........ = 0
      nes->ppu.t = (nes->ppu.t & ~0b011111100000000) | ((d & 0b111111) << 8);
    } else {
      // second write
      // t: ....... HGFEDCBA = d: HGFEDCBA
      // v                   = t
      nes->ppu.t = (nes->ppu.t & ~0b11111111) | d;
      nes->ppu.v = nes->ppu.t;
    }
    nes->ppu.w = !nes->ppu.w;
    break;
  }
  case 0x2007: // PPUDATA
    ppu_write(nes, nes->ppu.v, value);
    ppu_incr_v(&nes->ppu);
    break;
  }
}

uint16_t nt_addr(Mapper *mapper, uint16_t addr) {
  switch (mapper_mirroring(mapper)) {
  case MIRRORING_HORIZONTAL:
    return 0x2800 <= addr ? 0x0800 + addr % 0x0400 : addr % 0x0400;
  case MIRRORING_VERTICAL:
    return addr % 0x0800;
  }
  return addr - 0x2000;
}

uint16_t pallete_addr(uint16_t addr) {
  // http://wiki.nesdev.com/w/index.php/PPU_palettes#Memory_Map
  uint16_t ret = addr % 32;
  return ret % 4 == 0 ? ret | 0x10 : ret;
}

uint8_t ppu_read(NES *nes, uint16_t addr) {
  if (addr <= 0x1FFF) {
    return mapper_read(nes->mapper, addr);

  } else if (addr <= 0x2FFF) {
    return nes->ppu.nt[nt_addr(nes->mapper, addr)];

  } else if (addr <= 0x3EFF) {
    return nes->ppu.nt[nt_addr(nes->mapper, addr - 0x1000)];

  } else if (addr <= 0x3FFF) {
    return nes->ppu.pallete[pallete_addr(addr)];
  }

  return 0;
}

void ppu_write(NES *nes, uint16_t addr, uint8_t value) {
  if (addr <= 0x1FFF) {
    mapper_write(nes->mapper, addr, value);

  } else if (addr <= 0x2FFF) {
    nes->ppu.nt[nt_addr(nes->mapper, addr)] = value;

  } else if (addr <= 0x3EFF) {
    nes->ppu.nt[nt_addr(nes->mapper, addr - 0x1000)] = value;

  } else if (addr <= 0x3FFF) {
    nes->ppu.pallete[pallete_addr(addr)] = value;
  }
}
