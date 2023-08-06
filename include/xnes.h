#ifndef XNES_H
#define XNES_H

#include "stdint.h"

struct xnes;
typedef struct xnes XNES;

typedef struct {
    void* state;
    void (*update_frame)(void* state, uint8_t *buf, uint64_t len);
    void (*clean_up)(void** state);
} XNESFrameRenderer;

XNES *xnes_new(XNESFrameRenderer *frame_renderer);

typedef enum {
  XNES_ROM_PARSE_ERROR_NONE,
  XNES_ROM_PARSE_ERROR_CAN_NOT_ALLOCATED,
  XNES_ROM_PARSE_ERROR_INVALID_MAGIC_NUMBER,
  XNES_ROM_PARSE_ERROR_PADDING,
} XNESROMParseError;

XNESROMParseError xnes_insert_cartridge(XNES *xnes, uint8_t *buf, uint64_t len);

void xnes_run_frame(XNES *);

void xnes_release(XNES *);

#endif // XNES_H
