#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

struct cpu {
  uint8_t A, X, Y;
  uint8_t S;
  uint8_t P; // cpu_status
  uint16_t PC;

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

void cpu_status_set(struct cpu *cpu, enum cpu_status s, bool v);

bool cpu_status_enabled(struct cpu *cpu, enum cpu_status s);

void cpu_status_set_zn(struct cpu *cpu, uint8_t v);

#endif // CPU_H
