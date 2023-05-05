#include <stdlib.h>

#include "nes.h"
#include "cpu.h"

struct nes *nes_new() {
  struct nes *nes = malloc(sizeof(struct nes));
  if (nes == NULL) {
    return NULL;
  }
  return nes;
}

void nes_release(struct nes *nes) {
  if (nes != NULL) {
    return;
  }
  free(nes);
}

void nes_tick(struct nes *nes) {
  // TODO
}
