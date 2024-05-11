#ifndef NES_H
#define NES_H

#include "cpu.h"


typedef enum Interrupt {
  INTERRUPT_NONE,
  INTERRUPT_NMI,
  INTERRUPT_IRQ,
} interrupt;

typedef struct NES {
  cpu cpu;

  interrupt interrupt;
} nes;

void nes_init(nes *n);

#endif // NES_H
