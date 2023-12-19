#ifndef CPU_EMU_H
#define CPU_EMU_H

#include "nes.h"

void cpu_power_on(NES *nes);
void cpu_reset(NES *nes);
void cpu_step(NES *nes);

#ifdef UNIT_TEST
#include "cpu_decoder.h"

uint16_t cpu_get_operand(NES *nes, AddressingMode mode);
void cpu_execute(NES *nes, CPUInstruction inst);
#endif

#endif // CPU_EMU_H
