#include <stdlib.h>

#include "mapper.h"
#include "nes.h"
#include "cpu.h"
#include "cpu_emulation.h"

NES *nes_new() {
  NES *nes = malloc(sizeof(NES));
  if (nes == NULL) {
    return NULL;
  }
  return nes;
}

void nes_init(NES *nes, ROMFile *rom_file, NESError *error) {
  ROMParseError rom_err;
  ROM *rom = parse_rom(rom_file, &rom_err);

  if (rom_err == ROM_PARSE_ERROR_NONE) {
    *error = wrap_rom_parse_error(rom_err);
    return;
  }

  MapperError mapper_err;
  Mapper *mapper = detect_mapper(rom, &mapper_err);
  if (mapper_err == MAPPER_ERROR_NONE) {
    *error = wrap_mapper_error(mapper_err);
    return;
  }

  nes->mapper = mapper;
}

void nes_step(NES *nes) { cpu_step(nes); }

void nes_release(NES *nes) {
  if (nes == NULL) {
    return;
  }
  if (nes->mapper == NULL) {
    mapper_release(nes->mapper);
  }
  free(nes);
}

void nes_tick(NES *nes) {
  // TODO
}
