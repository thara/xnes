#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

#include <rom.h>

struct mapper;
typedef struct mapper Mapper;

typedef enum{
    horizontal,
    vertical,
} MirroringMode;

typedef enum {
    mapper_error_none,
    mapper_error_unsupported_mapper,
} MapperError;

void detect_mapper(ROM *rom, Mapper *mapper, MapperError* error);

uint8_t mapper_read(Mapper *mapper, uint16_t addr);

void mapper_write(Mapper *mapper, uint16_t addr, uint8_t value);

#endif // MAPPER_H
