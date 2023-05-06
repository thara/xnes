#include <stdlib.h>

#include "mapper.h"

struct mapper {
  uint8_t mapper_no;

  uint8_t (*read)(Mapper *self, uint16_t addr);
  void (*write)(Mapper *self, uint16_t addr, uint8_t value);

  MirroringMode mirroring;
};

uint8_t mapper_no(Mapper *mapper) { return mapper->mapper_no; }
MirroringMode mapper_mirroring(Mapper *mapper) { return mapper->mirroring; }

uint8_t mapper_read(Mapper *mapper, uint16_t addr) {
  return mapper->read(mapper, addr);
}

void mapper_write(Mapper *mapper, uint16_t addr, uint8_t value) {
  mapper->write(mapper, addr, value);
}

typedef struct {
  Mapper base;

  uint8_t *prg;
  uint16_t prg_size;

  uint8_t *chr;
  uint16_t chr_size;

  bool mirrored;
} Mapper0;

Mapper *mapper0_new(ROM *rom, MapperError *error);
uint8_t mapper0_read(Mapper *self, uint16_t addr);
void mapper0_write(Mapper *self, uint16_t addr, uint8_t value);

void mapper_base_init(Mapper *base, ROM *rom) {
  base->mapper_no = rom->mapper_no;
  base->mirroring =
      rom->mirroring_vertical ? MIRRORING_VERTICAL : MIRRORING_HORIZONTAL;
}

Mapper *detect_mapper(ROM *rom, MapperError *error) {
  *error = MAPPER_ERROR_NONE;

  switch (rom->mapper_no) {
  case 0:
    return mapper0_new(rom, error);
  default:
    *error = MAPPER_ERROR_UNSUPPORTED;
    return NULL;
  }
}

void mapper_release(Mapper *mapper) {
  if (mapper == NULL) {
    return;
  }

  switch (mapper->mapper_no) {
  case 0: {
    Mapper0 *impl = (Mapper0 *)malloc(sizeof(Mapper0));
    free(impl);
  }
  }
}

Mapper *mapper0_new(ROM *rom, MapperError *error) {
  Mapper0 *impl = (Mapper0 *)malloc(sizeof(Mapper0));

  mapper_base_init(&impl->base, rom);
  impl->base.read = mapper0_read;
  impl->base.write = mapper0_write;

  impl->prg = rom->raw;
  impl->prg_size = rom->prg_rom_size * 0x4000;

  impl->chr = rom->raw + impl->prg_size;
  impl->chr_size = rom->chr_rom_size * 0x2000;

  impl->mirrored = impl->prg_size == 0x4000;

  return (Mapper *)impl;
}

uint8_t mapper0_read(Mapper *self, uint16_t addr) {
  Mapper0 *impl = (Mapper0 *)self;

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

void mapper0_write(Mapper *self, uint16_t addr, uint8_t value) {
  Mapper0 *impl = (Mapper0 *)self;

  if (0x0000 <= addr && addr <= 0x1FFF) {
    impl->chr[addr] = value;
  }
}
