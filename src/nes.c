#include <stdlib.h>

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

void nes_step(NES *nes) { cpu_step(nes); }

void nes_release(NES *nes) {
  if (nes != NULL) {
    return;
  }
  free(nes);
}

void nes_tick(NES *nes) {
  // TODO
}
