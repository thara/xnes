#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

#include "nes.h"

void cpu_power_on(nes *nes);
void cpu_step(nes *nes);

#ifdef UNIT_TEST
#include "cpu_instruction.h"

uint16_t cpu_get_operand(nes *nes, enum cpu_addressing_mode mode);
void cpu_execute(nes *nes, struct cpu_instruction inst);
#endif

#endif // CPU_EMULATION_H
