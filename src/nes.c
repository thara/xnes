#include <stdlib.h>

#include "nes.h"
#include "cpu.h"

nes *nes_new() {
  nes *nes = malloc(sizeof(*nes));
  if (nes == NULL) {
    return NULL;
  }
  return nes;
}

void nes_release(nes *nes) {
  if (nes != NULL) {
    return;
  }
  free(nes);
}

void nes_tick(nes *nes) {
  // TODO
}
