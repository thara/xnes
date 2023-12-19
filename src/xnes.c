#include <stdlib.h>
#include <stdio.h>

#include "xnes.h"

#include "nes.h"
#include "nes_emu.h"

struct xnes {
  NES *nes;

  Cartridge *cart;
};

XNES *xnes_new(XNESFrameRenderer *frame_renderer) {
  struct xnes *xnes = calloc(1, sizeof(struct xnes));
  xnes->nes = nes_new();
  xnes->nes->renderer.state = frame_renderer->state;
  if (frame_renderer->update_frame != NULL) {
    xnes->nes->renderer.update_frame = frame_renderer->update_frame;
  }
  if (frame_renderer->clean_up != NULL) {
    xnes->nes->renderer.clean_up = frame_renderer->clean_up;
  }
  return xnes;
}

XNESROMParseError xnes_insert_cartridge(XNES *xnes, uint8_t *buf,
                                        uint64_t len) {
  xnes->cart = load_cartridge(buf, len);
  if (xnes->cart->rom_error == ROM_PARSE_ERROR_NONE) {
    nes_insert_cartridge(xnes->nes, xnes->cart->mapper);

    char str[256];
    mapper_info(xnes->cart->mapper, str, sizeof(str));
    printf("%s\n", str);
  }
  return (XNESROMParseError)xnes->cart->rom_error;
}

void xnes_init(XNES *xnes) {
  nes_power_on(xnes->nes);
  nes_reset(xnes->nes);
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
