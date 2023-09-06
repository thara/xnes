#include <stdlib.h>
#include <string.h>

#include "mapper.h"

const uint8_t header_size = 16;

struct mapper {
  uint8_t mapper_no;

  uint8_t (*read)(Mapper *self, uint16_t addr);
  void (*write)(Mapper *self, uint16_t addr, uint8_t value);
  void (*info)(Mapper *self, uint8_t *buf, uint64_t len);

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

void mapper_info(Mapper *mapper, char *buf, uint64_t len) {
  mapper->info(mapper, buf, len);
}

void get_mirroring_str(MirroringMode mirroring, char *out);

typedef struct {
  Mapper base;

  uint8_t *prg;
  uint16_t prg_size;

  uint8_t *chr;
  uint16_t chr_size;

  bool mirrored;
} Mapper0;

Mapper *mapper0_new(ROM *rom);
uint8_t mapper0_read(Mapper *self, uint16_t addr);
void mapper0_write(Mapper *self, uint16_t addr, uint8_t value);
void mapper0_info(Mapper *self, char *buf, uint64_t len);

void mapper_base_init(Mapper *base, ROM *rom) {
  base->mapper_no = rom->mapper_no;
  base->mirroring =
      rom->mirroring_vertical ? MIRRORING_VERTICAL : MIRRORING_HORIZONTAL;
}

Mapper *detect_mapper(ROM *rom) {
  switch (rom->mapper_no) {
  case 0:
    return mapper0_new(rom);
  default:
    return NULL;
  }
}

void mapper_release(Mapper *mapper) {
  if (mapper == NULL) {
    return;
  }

  switch (mapper->mapper_no) {
  case 0: {
    Mapper0 *impl = (Mapper0 *)mapper;
    free(impl);
  }
  }
}

Mapper *mapper0_new(ROM *rom) {
  Mapper0 *impl = (Mapper0 *)malloc(sizeof(Mapper0));

  mapper_base_init(&impl->base, rom);
  impl->base.read = mapper0_read;
  impl->base.write = mapper0_write;
  impl->base.info = mapper0_info;

  impl->prg = rom->raw + header_size;
  impl->prg_size = rom->prg_rom_size * 0x4000;

  impl->chr = impl->prg + impl->prg_size;
  impl->chr_size = rom->chr_rom_size * 0x2000;

  impl->mirrored = impl->prg_size == 0x4000;

  return (Mapper *)impl;
}

#include <stdio.h>

uint8_t mapper0_read(Mapper *self, uint16_t addr) {
  Mapper0 *impl = (Mapper0 *)self;

  if (addr <= 0x1FFF) {
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

  if (addr <= 0x1FFF) {
    impl->chr[addr] = value;
  }
}

void mapper0_info(Mapper *self, char *buf, uint64_t len) {
  Mapper0 *impl = (Mapper0 *)self;

  char mirroring_str[10];
  get_mirroring_str(self->mirroring, mirroring_str);

  snprintf(buf, len,
           "mapper 0:\n\tPRG: 0x%x byte\n\tCHR: 0x%x byte\n\tmirroring: "
           "%s\n\tmirrored: %s",
           impl->prg_size, impl->chr_size, mirroring_str,
           impl->mirrored ? "true" : "false");
}

typedef struct {
  Mapper base;
} MockMapper;

uint8_t mock_mapper_read(Mapper *self, uint16_t addr) { return 0; }
void mock_mapper_write(Mapper *self, uint16_t addr, uint8_t value) { /* NOP */
}

Mapper *mock_mapper_new(uint8_t mapper_no, MirroringMode mirroring) {
  MockMapper *impl = (MockMapper *)malloc(sizeof(MockMapper));
  impl->base.mapper_no = mapper_no;
  impl->base.mirroring = mirroring;
  impl->base.read = mock_mapper_read;
  impl->base.write = mock_mapper_write;
  return (Mapper *)impl;
}

void get_mirroring_str(MirroringMode mirroring, char *out) {
  switch (mirroring) {
  case MIRRORING_HORIZONTAL:
    strcpy(out, "Horizontal");
    break;
  case MIRRORING_VERTICAL:
    strcpy(out, "Vertical");
    break;
  default:
    strcpy(out, "(unsupported)");
    break;
  }
}
