#include "error.h"

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

void as_rom_parse_error(NESError nes_error, ROMParseError *error) {
  if (NES_ERROR_ROM_PARSE <= nes_error && nes_error < NES_ERROR_MAPPER) {
    *error = nes_error - NES_ERROR_ROM_PARSE;
  }
}

void as_mapper_error(NESError nes_error, MapperError *error) {
  if (NES_ERROR_MAPPER <= nes_error) {
    *error = nes_error - NES_ERROR_MAPPER;
  }
}
