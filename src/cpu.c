#include <stdlib.h>

#include "cpu.h"

void cpu_status_set(CPU *cpu, CPUStatus s, bool v) {
  cpu->P &= ~(1 << s);
  cpu->P |= v << s;
}

bool cpu_status_enabled(CPU *cpu, CPUStatus s) {
  return ((cpu->P >> s) & 1) == 1;
}

void cpu_status_set_zn(CPU *cpu, uint8_t v) {
  cpu_status_set(cpu, CPU_STATUS_Z, v == 0);
  cpu_status_set(cpu, CPU_STATUS_N, (v & 0x80) == 0x80);
}
