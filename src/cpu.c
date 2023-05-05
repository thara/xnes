#include <stdlib.h>

#include "cpu.h"

void cpu_status_set(struct cpu *cpu, enum cpu_status s, bool v) {
  cpu->P &= ~(1 << s);
  cpu->P |= v << s;
}

bool cpu_status_enabled(struct cpu *cpu, enum cpu_status s) {
  return ((cpu->P >> s) & 1) == 1;
}

void cpu_status_set_zn(struct cpu *cpu, uint8_t v) {
  cpu_status_set(cpu, Z, v == 0);
  cpu_status_set(cpu, N, (v & 0x80) == 0x80);
}
