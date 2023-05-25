#include "ppu_emulation.h"
#include "ppu.h"

uint8_t ppu_read(NES *nes, uint16_t addr);
void ppu_write(NES *nes, uint16_t addr, uint8_t value);

uint8_t ppu_read_register(NES *nes, uint16_t addr) {
  uint8_t ret;

  switch (addr) {
  case 0x2002:
    // PPUSTATUS
    ret = nes->ppu.status | (nes->ppu.cpu_data_bus & 0b11111);
    ppu_status_set(&nes->ppu, PPUSTATUS_VBLANK, false);
    nes->ppu.w = true;
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
