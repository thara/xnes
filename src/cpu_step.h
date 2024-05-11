#ifndef CPU_STEP_H
#define CPU_STEP_H

#include "nes.h"

void cpu_power_on(nes *n);

void cpu_reset(nes *n);

void cpu_step(nes *n);

#endif // CPU_STEP_H
