#ifndef NES_H
#define NES_H

#include "cpu.h"

typedef struct {
    struct cpu cpu;
} nes;

nes* nes_new();

void nes_release(nes* nes);

void nes_tick(nes* nes);

#endif // NES_H

