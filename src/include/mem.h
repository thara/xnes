#ifndef MEM_H
#define MEM_H

#include <stdint.h>

#include "nes.h"

uint8_t mem_read(struct nes *nes, uint16_t addr);

void mem_write(struct nes *nes, uint16_t addr, uint8_t val);

#endif // MEM_H

