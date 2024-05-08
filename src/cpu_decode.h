#ifndef CPU_DECODE_H
#define CPU_DECODE_H

#include <stdint.h>

// https://www.nesdev.org/wiki/6502_instructions

// clang-format off

// https://wiki.nesdev.org/w/index.php?title=CPU_addressing_modes
typedef enum AddressingMode {
  IMPLICIT, ACCUMULATOR, IMMEDIATE,
  ZERO_PAGE, ZERO_PAGE_X, ZERO_PAGE_Y,
  ABSOLUTE, ABSOLUTE_X, ABSOLUTE_X_WITH_PENALTY, ABSOLUTE_Y, ABSOLUTE_Y_WITH_PENALTY,
  RELATIVE,
  INDIRECT, INDEXED_INDIRECT, INDIRECT_INDEXED, INDIRECT_INDEXED_WITH_PENALTY,
} addressing_mode;

typedef enum Mnemonic {
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
} mnemonic;

// clang-format on

typedef struct CPUInstruction {
  mnemonic mnemonic;
  addressing_mode mode;
} cpu_instruction;

cpu_instruction cpu_decode(uint8_t opcode);

#endif // CPU_DECODE_H

