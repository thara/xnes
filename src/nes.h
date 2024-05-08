#ifndef NES_H
#define NES_H

#include "cpu.h"

typedef struct NES {
  cpu cpu;
} nes;

void nes_init(nes *n);

#endif // NES_H
