#ifndef CPU_H
#define CPU_H

struct nes;
struct cpu;

void cpu_init(struct nes *nes);
void cpu_step(struct nes *nes);

#endif // CPU_H
