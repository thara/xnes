#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

struct cpu {
  // https://wiki.nesdev.org/w/index.php?title=CPU_registers

  // Accumulator, Index X/Y register
  uint8_t A, X, Y;
  // Stack pointer
  uint8_t S;
  // Status register
  uint8_t P; // cpu_status
  // Program counter
  uint16_t PC;

  // clock cycle
  uintmax_t cycles;
};

enum cpu_status {
  C, // Carry
  Z, // Zero
  I, // Interrupt Disable
  D, // Decimal
  V, // Overflow
  N, // Negative
};

struct cpu* cpu_new();

void cpu_status_set(struct cpu *cpu, enum cpu_status s, bool v);

bool cpu_status_enabled(struct cpu *cpu, enum cpu_status s);

void cpu_status_set_zn(struct cpu *cpu, uint8_t v);

#endif // CPU_H
