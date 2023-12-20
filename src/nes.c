#include <stdlib.h>

#include "nes.h"

#include "mapper.h"

void nop_update_frame(void *state, uint8_t *buf, uint64_t len) { return; }
void nop_clean_up(void **state) { return; }

NES *nes_new() {
  NES *nes = calloc(1, sizeof(NES));
  if (nes == NULL) {
    return NULL;
  }
  nes->interrupt = INTERRUPT_NONE;
  nes->renderer.update_frame = nop_update_frame;
  nes->renderer.clean_up = nop_clean_up;
  return nes;
}

void nes_release(NES *nes) {
  if (nes == NULL) {
    return;
  }
  if (nes->mapper != NULL) {
    mapper_release(nes->mapper);
  }
  nes->renderer.clean_up(nes->renderer.state);
  free(nes);
}
