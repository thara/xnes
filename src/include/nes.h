#ifndef NES_H
#define NES_H

#include "cpu.h"

struct nes {
    struct cpu cpu;
};

struct nes* nes_new();

void nes_release(struct nes* nes);

void nes_tick(struct nes* nes);

#endif // NES_H

