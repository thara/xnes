#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

#include "nes.h"

void cpu_power_on(struct nes *nes);
void cpu_step(struct nes *nes);

#ifdef UNIT_TEST
#include "cpu_instruction.h"

uint16_t cpu_get_operand(struct nes *nes, enum cpu_addressing_mode mode);
void cpu_execute(struct nes *nes, struct cpu_instruction inst);
#endif

#endif // CPU_EMULATION_H
