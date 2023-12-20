#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

#define SPRITE_COUNT 64
#define SPRITE_LIMIT 8

typedef struct {
  bool enabled;
  uint8_t index;

  uint8_t x;    // X position of left
  uint8_t y;    // Y position of top
  uint8_t tile; // tile index number
  uint8_t attr; // SpriteAttr

  uint8_t low, high;
} Sprite;

typedef struct {
  // registers
  uint8_t ctrl;     // PPUCTRL
  uint8_t mask;     // PPUMASK
  uint8_t status;   // PPUSTATUS
  uint8_t data;     // PPUDATA
  uint8_t oam_addr; // OAMADDR

  uint16_t v, t; // current/temporary VRAM address
  uint8_t x;     // fine x scroll
  bool w;        // first or second write toggle

  uint8_t nt[0x1000];
  uint8_t pallete[0x0020];

  uint8_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

  uint8_t cpu_data_bus;

  struct {
    uint16_t addr; // temp addr
    uint8_t nt;    // name table byte
    uint8_t at;    // attribute table byte

    uint16_t low, high;
    uint16_t shift_low, shift_high;
    uint8_t attr_shift_low, attr_shift_high;
    bool attr_latch_low, attr_latch_high;
  } bg;

  struct {
    uint8_t oam[4 * SPRITE_COUNT]; // https://www.nesdev.org/wiki/PPU_OAM

    Sprite primary[8];
    Sprite secondary[8];
  } spr;

  struct {
    uint16_t line; // 0 ..= 261
    uint16_t dot;  // 0 ..= 340
  } scan;

  uintmax_t frames;
} PPU;

void ppu_power_on(PPU *ppu);
void ppu_reset(PPU *ppu);

typedef enum {
  PPUCTRL_NT = 0b11,          // Base nametable address
  PPUCTRL_VRAM_INCR = 1 << 2, // VRAM address increment
  PPUCTRL_SPR_TABLE = 1 << 3, // Sprite pattern table address for 8x8 sprites
  PPUCTRL_BG_TABLE = 1 << 4,  // Background pattern table address
  PPUCTRL_SPR_8x16 = 1 << 5,  // Sprite size
  PPUCTRL_SLAVE = 1 << 6,     // PPU master/slave (0: master, 1: slave)
  PPUCTRL_NMI = 1 << 7,
} PPUCTRL;

typedef enum {
  PPUMASK_GRAYSCALE = 1,     // Greyscale
  PPUMASK_BG_LEFT = 1 << 1,  // Show background in leftmost 8 pixels
  PPUMASK_SPR_LEFT = 1 << 2, // Show sprite in leftmost 8 pixels
  PPUMASK_BG = 1 << 3,       // Show background
  PPUMASK_SPR = 1 << 4,      // Show sprite
  PPUMASK_RED = 1 << 5,      // Emphasize red
  PPUMASK_GREEN = 1 << 6,    // Emphasize green
  PPUMASK_BLUE = 1 << 7,     // Emphasize blue
} PPUMASK;

typedef enum {
  PPUSTATUS_SPR_OVERFLOW = 1 << 5, // Sprite overflow
  PPUSTATUS_SPR_0_HIT = 1 << 6,    // Sprite 0 Hit
  PPUSTATUS_VBLANK = 1 << 7,       // In vblank?
} PPUSTATUS;

typedef enum {
  SPRITE_ATTR_PALETTE = 0b11,
  SPRITE_ATTR_BEHIND_BG = 1 << 5,
  SPRITE_ATTR_FLIP_HORIZONTALLY = 1 << 6,
  SPRITE_ATTR_FLIP_VERTICALLY = 1 << 7,
} SpriteAttr;

#endif // PPU_H
