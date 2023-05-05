#ifndef NES_H
#define NES_H

#include "cpu.h"

typedef struct {
    cpu cpu;
} NES;

NES* nes_new();

void nes_release(NES* nes);

void nes_tick(NES* nes);

#endif // NES_H

