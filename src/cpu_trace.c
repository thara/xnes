#include "cpu_trace.h"

#include <stdio.h>
#include <string.h>

#include "cpu_decoder.h"
#include "mem.h"

uint8_t instruction_length(AddressingMode mode);

CPUTrace cpu_trace(NES *nes) {
  CPUTrace trace = {0};
  trace.current_state = nes->cpu;

  uint8_t op = mem_read(nes, nes->cpu.PC);
  trace.next_opcode = op;

  trace.next_instruction = cpu_decode(op);

  uint8_t len = instruction_length(trace.next_instruction.mode);
  switch (len) {
  case 3:
    trace.next_operand2 = mem_read(nes, nes->cpu.PC + 2);
  case 2:
    trace.next_operand1 = mem_read(nes, nes->cpu.PC + 1);
  }
  return trace;
}

uint8_t instruction_length(AddressingMode mode) {
  switch (mode) {
  case IMMEDIATE:
  case ZERO_PAGE:
  case ZERO_PAGE_X:
  case ZERO_PAGE_Y:
  case RELATIVE:
  case INDIRECT_INDEXED:
  case INDEXED_INDIRECT:
  case INDIRECT_INDEXED_WITH_PENALTY:
    return 2;
  case INDIRECT:
  case ABSOLUTE:
  case ABSOLUTE_X:
  case ABSOLUTE_X_WITH_PENALTY:
  case ABSOLUTE_Y:
  case ABSOLUTE_Y_WITH_PENALTY:
    return 3;
  default:
    return 1;
  }
}

void assemble_cpu_trace(CPUTrace *trace, char *str) {
  uint8_t len = instruction_length(trace->next_instruction.mode);

  char op[9];
  switch (len) {
  case 1:
    sprintf(op, "%02X      ", trace->next_opcode);
  case 2:
    sprintf(op, "%02X %02X   ", trace->next_opcode, trace->next_operand1);
  case 3:
    sprintf(op, "%02X %02X %02X", trace->next_opcode, trace->next_operand1,
            trace->next_operand2);
  };

  snprintf(str, sizeof(str), "%04X  %s  A:%02X X:%02X Y:%02X P:%02X SP:%02X",
           trace->current_state.PC, op, trace->current_state.A,
           trace->current_state.X, trace->current_state.Y,
           trace->current_state.P, trace->current_state.S);
}

// nestest.log line example:
// C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD
// PPU:  0, 21 CYC:7
//
bool parse_cpu_trace(char *str, CPUTrace *dst) {
  int n = sscanf(str, "%04hX", &dst->current_state.PC);
  int offset = 6;

  n += sscanf(str + offset, "%02hhX", &dst->next_opcode);
  offset += 3;

  char operand1[3] = {'\0'};
  char operand2[3] = {'\0'};
  for (int i = 0; i < 6; i++) {
    char c = *(str + offset + i);
    if (c != ' ') {
      if (i < 3) {
        operand1[i] = c;
      } else {
        operand2[i - 3] = c;
      }
    }
  }
  if (strlen(operand1) == 2 && operand1[0] != ' ' && operand1[1] != ' ') {
    sscanf(operand1, "%02hhX", &dst->next_operand1);
  }
  if (strlen(operand2) == 2 && operand2[0] != ' ' && operand2[1] != ' ') {
    sscanf(operand2, "%02hhX", &dst->next_operand2);
  }

  n += 2;
  offset += 39;

  n += sscanf(str + offset, "A:%02hhX X:%02hhX Y:%02hhX P:%02hhX SP:%02hhX",
              &dst->current_state.A, &dst->current_state.X,
              &dst->current_state.Y, &dst->current_state.P,
              &dst->current_state.S);
  if (n != 9) {
    return false;
  }
  offset += 26;

  // skip PPU state
  offset += 12;

  n += sscanf(str + offset, "CYC:%ju", &dst->current_state.cycles);
  if (n != 10) {
    return false;
  }

  dst->next_instruction = cpu_decode(dst->next_opcode);
  return true;
}
