#ifndef CPU_INSTRUCTION_H
#define CPU_INSTRUCTION_H

#include <stdint.h>

// https://www.nesdev.org/wiki/6502_instructions

// clang-format off

// https://wiki.nesdev.org/w/index.php?title=CPU_addressing_modes
typedef enum {
  implicit, accumulator, immediate,
  zero_page, zero_page_x, zero_page_y,
  absolute, absolute_x, absolute_x_with_penalty, absolute_y, absolute_y_with_penalty,
  relative,
  indirect, indexed_indirect, indirect_indexed, indirect_indexed_with_penalty,
} AddressingMode;

typedef enum {
  // Load/Store Operations,
  LDA, LDX, LDY, STA, STX, STY,
  // Register Operations,
  TAX, TSX, TAY, TXA, TXS, TYA,
  // Stack instructions,
  PHA, PHP, PLA, PLP,
  // Logical instructions,
  AND, EOR, ORA, BIT,
  // Arithmetic instructions,
  ADC, SBC, CMP, CPX, CPY,
  // Increment/Decrement instructions,
  INC, INX, INY, DEC, DEX, DEY,
  // Shift instructions,
  ASL, LSR, ROL, ROR,
  // Jump instructions,
  JMP, JSR, RTS, RTI,
  // Branch instructions,
  BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS,
  // Flag control instructions,
  CLC, CLD, CLI, CLV, SEC, SED, SEI,
  // Misc,
  BRK, NOP,
  // Unofficial,
  LAX, SAX, DCP, ISB, SLO, RLA, SRE, RRA,
} Mnemonic;

// clang-format on

typedef struct {
  Mnemonic mnemonic;
  AddressingMode mode;
} CPUInstruction;

CPUInstruction cpu_decode(uint8_t opcode);

#endif // CPU_INSTRUCTION_H

