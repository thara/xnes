#include <stdlib.h>

#include "nes.h"
#include "cpu.h"

struct nes *nes_new() {
  struct nes *nes = malloc(sizeof(struct nes));
  if (nes == NULL) {
    return NULL;
  }
  nes->cpu = calloc(1, sizeof(struct cpu));
  return nes;
}

void nes_release(struct nes *nes) {
  if (nes != NULL) {
    return;
  }
  if (nes->cpu != NULL) {
    free(nes->cpu);
  }
  free(nes);
}

void nes_tick(struct nes *nes) {
  // TODO
}
