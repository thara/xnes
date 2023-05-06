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

NESError wrap_rom_parse_error(ROMParseError error);
NESError wrap_mapper_error(MapperError error);

void nes_init(NES *nes, ROMFile *rom_file, NESError *error) {
  ROM rom;
  ROMParseError rom_err;
  parse_rom(rom_file, &rom, &rom_err);

  if (rom_err == ROM_PARSE_ERROR_NONE) {
    *error = wrap_rom_parse_error(rom_err);
    return;
  }

  MapperError mapper_err;
  Mapper *mapper = detect_mapper(&rom, &mapper_err);
  if (mapper_err == MAPPER_ERROR_NONE) {
    *error = wrap_mapper_error(mapper_err);
    return;
  }

  nes->mapper = mapper;
}

typedef enum {
  NES_ERROR_NONE,
  NES_ERROR_ROM_PARSE = 1000,
  NES_ERROR_MAPPER = 2000,
} NESErrorInternal;

NESError wrap_rom_parse_error(ROMParseError error) {
  return NES_ERROR_ROM_PARSE + error;
}

NESError wrap_mapper_error(MapperError error) {
  return NES_ERROR_MAPPER + error;
}

void nes_as_rom_parse_error(NESError nes_error, ROMParseError *error) {
  if (NES_ERROR_ROM_PARSE <= nes_error && nes_error < NES_ERROR_MAPPER) {
    *error = nes_error - NES_ERROR_ROM_PARSE;
  }
}

void nes_as_mapper_error(NESError nes_error, MapperError *error) {
  if (NES_ERROR_MAPPER <= nes_error) {
    *error = nes_error - NES_ERROR_MAPPER;
  }
}

void nes_step(NES *nes) { cpu_step(nes); }

void nes_release(NES *nes) {
  if (nes != NULL) {
    return;
  }
  mapper_release(nes->mapper);
  free(nes);
}

void nes_tick(NES *nes) {
  // TODO
}
