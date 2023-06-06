#ifndef ERROR_H
#define ERROR_H

#include "mapper.h"
#include "rom.h"

typedef int NESError;

extern const NESError NES_ERROR_NONE;

NESError wrap_rom_parse_error(ROMParseError error);
NESError wrap_mapper_error(MapperError error);

void as_rom_parse_error(NESError nes_error, ROMParseError *error);
void as_mapper_error(NESError nes_error, MapperError *error);

#endif // ERROR_H
