#include <stdlib.h>

#include "xnes.h"

#include "nes.h"

struct xnes {
  NES *nes;

  Cartridge *cart;
};

XNES *xnes_new(XNESFrameRenderer *frame_renderer) {
  struct xnes *xnes = calloc(1, sizeof(struct xnes));
  xnes->nes = nes_new();
  xnes->nes->renderer.state = frame_renderer->state;
  xnes->nes->renderer.update_frame = frame_renderer->update_frame;
  xnes->nes->renderer.clean_up = frame_renderer->clean_up;
  return xnes;
}

XNESROMParseError xnes_insert_cartridge(XNES *xnes, uint8_t *buf,
                                        uint64_t len) {
  xnes->cart = load_cartridge(buf, len);
  return (XNESROMParseError)xnes->cart->rom_error;
}

void xnes_run_frame(XNES *xnes) {
  uintmax_t frames = xnes->nes->ppu.frames;
  while (frames == xnes->nes->ppu.frames) {
    nes_step(xnes->nes);
  }
}

void xnes_release(XNES *xnes) {
  if (xnes == NULL) {
    return;
  }
  if (xnes->nes != NULL) {
    nes_release(xnes->nes);
  }
  if (xnes->cart != NULL) {
    cartridge_release(xnes->cart);
  }
  free(xnes);
}
