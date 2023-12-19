#ifndef NES_H
#define NES_H

#include "stdint.h"

#include "cpu.h"
#include "ppu.h"
#include "mapper.h"
#include "rom.h"
#include "input.h"

typedef enum {
  INTERRUPT_NONE,
  INTERRUPT_NMI,
  INTERRUPT_IRQ,
} Interrupt;

typedef struct {
  CPU cpu;
  uint8_t wram[0x0800];

  PPU ppu;

  Interrupt interrupt;

  Mapper *mapper;

  Input *input1;
  Input *input2;

  struct {
    void* state;
    void (*update_frame)(void* state, uint8_t *buf, uint64_t len);
    void (*clean_up)(void** state);
  } renderer;
} NES;

NES *nes_new();

void nes_release(NES *nes);

#endif // NES_H
