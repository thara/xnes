#ifndef NES_H
#define NES_H

#include "stdint.h"

#include "cpu.h"

typedef struct {
    CPU cpu;

    uint8_t wram[0x0800];
} NES;

NES* nes_new();

void nes_step(NES* nes);

void nes_release(NES* nes);

void nes_tick(NES* nes);

#endif // NES_H

