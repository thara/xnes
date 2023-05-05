#ifndef CPU_EMULATION_H
#define CPU_EMULATION_H

#include "nes.h"

void cpu_power_on(NES *nes);
void cpu_step(NES *nes);

#ifdef UNIT_TEST
#include "cpu_instruction.h"

uint16_t cpu_get_operand(NES *nes, AddressingMode mode);
void cpu_execute(NES *nes, CPUInstruction inst);
#endif

#endif // CPU_EMULATION_H
