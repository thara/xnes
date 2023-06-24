#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

#include "rom.h"

struct mapper;
typedef struct mapper Mapper;

typedef enum {
  MIRRORING_HORIZONTAL,
  MIRRORING_VERTICAL,
} MirroringMode;

Mapper *detect_mapper(ROM *rom);

uint8_t mapper_no(Mapper *mapper);
MirroringMode mapper_mirroring(Mapper *mapper);

uint8_t mapper_read(Mapper *mapper, uint16_t addr);

void mapper_write(Mapper *mapper, uint16_t addr, uint8_t value);

void mapper_release(Mapper *mapper);

#ifdef UNIT_TEST
Mapper *mock_mapper_new(uint8_t mapper_no, MirroringMode mirroring);
#endif

#endif // MAPPER_H
