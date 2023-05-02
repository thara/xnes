#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

#include "nes.h"

void cpu_init(struct nes *nes);
void cpu_step(struct nes *nes);

#endif // CPU_EMULATION_H
