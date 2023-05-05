#include <stdlib.h>

#include "mapper.h"

struct mapper {
  uint8_t (*read)(struct mapper *self, uint16_t addr);
  void (*write)(struct mapper *self, uint16_t addr, uint8_t value);
  mirroring mirroring;
};

uint8_t mapper_read(struct mapper *mapper, uint16_t addr) {
  return mapper->read(mapper, addr);
}

void mapper_write(struct mapper *mapper, uint16_t addr, uint8_t value) {
  mapper->write(mapper, addr, value);
}

typedef struct {
  struct mapper *base;

  uint8_t *prg;
  uint16_t prg_size;

  uint8_t *chr;
  uint16_t chr_size;

  bool mirrored;
} mapper0;

void mapper0_init(rom *rom, struct mapper *mapper, mapper_error *error);
uint8_t mapper0_read(struct mapper *self, uint16_t addr);
void mapper0_write(struct mapper *self, uint16_t addr, uint8_t value);

void detect_mapper(rom *rom, struct mapper *mapper, mapper_error *error) {
  switch (rom->mapper_no) {
  case 0:
    mapper0_init(rom, mapper, error);
    break;
  default:
    *error = mapper_error_unsupported_mapper;
    return;
  }
}

void mapper0_init(rom *rom, struct mapper *mapper, mapper_error *error) {
  mapper0 *impl = (mapper0 *)malloc(sizeof(mapper0));
  impl->base = malloc(sizeof(struct mapper));
  impl->base->read = mapper0_read;
  impl->base->write = mapper0_write;
  impl->base->mirroring = rom->mirroring_vertical ? vertical : horizontal;

  impl->prg = rom->raw;
  impl->prg_size = rom->prg_rom_size * 0x4000;

  impl->chr = rom->raw + impl->prg_size;
  impl->chr_size = rom->chr_rom_size * 0x2000;

  impl->mirrored = impl->prg_size == 0x4000;

  mapper = (struct mapper *)impl;
}

uint8_t mapper0_read(struct mapper *self, uint16_t addr) {
  mapper0 *impl = (mapper0 *)self;

  if (0x0000 <= addr && addr <= 0x1FFF) {
    return impl->chr[addr];
  } else if (0x8000 <= addr && addr <= 0xFFFF) {
    if (impl->mirrored) {
      addr %= 0x4000;
    } else {
      addr -= 0x8000;
    }
    return impl->prg[addr];
  }
  return 0;
}

void mapper0_write(struct mapper *self, uint16_t addr, uint8_t value) {
  mapper0 *impl = (mapper0 *)self;

  if (0x0000 <= addr && addr <= 0x1FFF) {
    impl->chr[addr] = value;
  }
}
