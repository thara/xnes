#include "ppu_render.h"

#include "ppu.h"
#include "ppu_io.h"

#include "stdio.h"

#define TILE_HEIGHT 8

// Notes in https://www.nesdev.org/w/images/default/d/d1/Ntsc_timing.png
#define PIXEL_DELAYED 2

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
  int16_t x = ppu->scan.dot - PIXEL_DELAYED;

  // visible
  if (ppu->scan.line < 240 && x < 256) {
    // background
    uint8_t bg = 0;
    if (ppu_mask_enabled(&nes->ppu, PPUMASK_BG) &&
        !(!ppu_mask_enabled(&nes->ppu, PPUMASK_BG_LEFT) && x < 8)) {
      bg = NTH(ppu->bg.shift_high, 15 - ppu->x) << 1 |
           NTH(ppu->bg.shift_low, 15 - ppu->x);
      /* if (0 < bg) { */
      /*     printf("[%d]bh:%d(%d)", ppu->scan.dot, ppu->bg.shift_high, ppu->bg.high); */
      /* } */
      /* if (0 < bg) { */
      /*   bg |= (NTH(ppu->bg.attr_shift_high, 7 - ppu->x) << 1 | */
      /*          NTH(ppu->bg.attr_shift_low, 7 - ppu->x)) */
      /*         << 2; */
      /* } */
    }
    // sprites
    uint8_t spr = 0;
    bool behind_bg = 0;
    /* if (ppu_mask_enabled(&nes->ppu, PPUMASK_SPR) && */
    /*     !(!ppu_mask_enabled(&nes->ppu, PPUMASK_SPR_LEFT) && x < 8)) { */
    /*   // https://www.nesdev.org/wiki/PPU_sprite_priority */
    /*   // Sprites with lower OAM indices are drawn in front */
    /*   for (int i = 7; 0 <= i; i--) { */
    /*     Sprite s = ppu->spr.primary[i]; */
    /*     if (!s.enabled) { */
    /*       continue; */
    /*     } */
    /*     uint16_t spr_x = x - s.x; */
    /*     if (8 <= spr_x) { */
    /*       continue; */
    /*     } */
    /*     if (0 < (s.attr & SPRITE_ATTR_FLIP_HORIZONTALLY)) { */
    /*       spr_x ^= 7; // horizontal flip */
    /*     } */
    /*     uint8_t px = 7 - spr_x; */
    /*     uint8_t palette = ((s.high >> px) & 1) << 1 | (s.low >> px) << 1; */
    /*     if (palette == 0) { */
    /*       continue; */
    /*     } */
    /*     if (i == 0 && bg != 0 && x != 255) { */
    /*       ppu_status_set(ppu, PPUSTATUS_SPR_0_HIT, true); */
    /*     } */
    /*     spr = (palette | (s.attr & SPRITE_ATTR_PALETTE) << 2) + 0x10; */
    /*     behind_bg = 0 < (s.attr & SPRITE_ATTR_BEHIND_BG); */
    /*   } */
    /* } */

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
    if (0 < color) {
        /* printf("%d %d %d\n", ppu->scan.line, x, color); */
        printf("%d(%d)", color, bg);
    }
    ppu->buffer[ppu->scan.line * 256 + x] = ppu_read(nes, 0x3F00 + color);
  }

  ppu_bg_shift(&nes->ppu);
}
