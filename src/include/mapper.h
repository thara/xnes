#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

#include <rom.h>

struct mapper;

typedef enum{
    horizontal,
    vertical,
} MirroringMode;

typedef enum {
    mapper_error_none,
    mapper_error_unsupported_mapper,
} mapper_error;

void detect_mapper(ROM *rom, struct mapper *mapper, mapper_error* error);

uint8_t mapper_read(struct mapper *mapper, uint16_t addr);

void mapper_write(struct mapper *mapper, uint16_t addr, uint8_t value);

#endif // MAPPER_H
