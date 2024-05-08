#include "cpu.h"

#include <stdlib.h>

void cpu_status_set(cpu *c, cpu_status s, bool v) {
  c->P &= ~(1 << s);
  c->P |= v << s;
}

bool cpu_status_enabled(cpu *c, cpu_status s) { return ((c->P >> s) & 1) == 1; }

void cpu_status_set_zn(cpu *c, uint8_t v) {
  cpu_status_set(c, CPU_STATUS_Z, v == 0);
  cpu_status_set(c, CPU_STATUS_N, (v & 0x80) == 0x80);
}
