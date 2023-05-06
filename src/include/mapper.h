#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

#include <rom.h>

struct mapper;
typedef struct mapper Mapper;

typedef enum{
    MIRRORING_HORIZONTAL,
    MIRRORING_VERTICAL,
} MirroringMode;

typedef enum {
    MAPPER_ERROR_NONE,
    MAPPER_ERROR_UNSUPPORTED,
} MapperError;

Mapper* detect_mapper(ROM *rom, MapperError* error);

uint8_t mapper_no(Mapper *mapper);
MirroringMode mapper_mirroring(Mapper *mapper);

uint8_t mapper_read(Mapper *mapper, uint16_t addr);

void mapper_write(Mapper *mapper, uint16_t addr, uint8_t value);

void mapper_release(Mapper *mapper);

#endif // MAPPER_H
