#ifndef CPU_TRACE_H
#define CPU_TRACE_H

#include "nes.h"
#include "cpu_decoder.h"

typedef struct {
  CPU current_state;

  CPUInstruction next_instruction;

  uint8_t next_opcode;
  uint8_t next_operand1;
  uint8_t next_operand2;
} CPUTrace;

CPUTrace cpu_trace(NES *nes);

void assemble_cpu_trace(CPUTrace *trace, char *str);
bool parse_cpu_trace(char *str, CPUTrace *dst);

#endif // CPU_TRACE_H
