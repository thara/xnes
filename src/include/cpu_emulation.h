#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

#include "nes.h"

void cpu_power_on(NES *nes);
void cpu_step(NES *nes);

#ifdef UNIT_TEST
#include "cpu_instruction.h"

uint16_t cpu_get_operand(NES *nes, cpu_addressing_mode mode);
void cpu_execute(NES *nes, cpu_instruction inst);
#endif

#endif // CPU_EMULATION_H
