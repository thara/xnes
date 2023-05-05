#ifndef MAPPER_H
#define MAPPER_H

#include <stdint.h>

#include <rom.h>

struct mapper;

enum mirroring{
    horizontal,
    vertical,
};

enum mapper_error {
    mapper_error_none,
    mapper_error_unsupported_mapper,
};

void detect_mapper(rom *rom, struct mapper *mapper, enum mapper_error* error);

uint8_t mapper_read(struct mapper *mapper, uint16_t addr);

void mapper_write(struct mapper *mapper, uint16_t addr, uint8_t value);

#endif // MAPPER_H
