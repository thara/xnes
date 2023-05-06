#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
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
} CPU;

typedef enum {
  CPU_STATUS_C = 0, // Carry
  CPU_STATUS_Z = 1, // Zero
  CPU_STATUS_I = 2, // Interrupt Disable
  CPU_STATUS_D = 3, // Decimal
  CPU_STATUS_V = 6, // Overflow
  CPU_STATUS_N = 7, // Negative
} CPUStatus;

CPU *cpu_new();

void cpu_status_set(CPU *cpu, CPUStatus s, bool v);

bool cpu_status_enabled(CPU *cpu, CPUStatus s);

void cpu_status_set_zn(CPU *cpu, uint8_t v);

#endif // CPU_H
