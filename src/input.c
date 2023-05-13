#include "input.h"

#include <stdbool.h>
#include <stdlib.h>

void input_write(Input *self, uint8_t state) { self->write(self, state); }

uint8_t input_read(Input *self) { return self->read(self); }

void standard_controller_write(Input *input, uint8_t state);
uint8_t standard_controller_read(Input *input);

StandardController *input_standard_controller_new() {
  StandardController *impl =
      (StandardController *)malloc(sizeof(StandardController));
  impl->input.write = standard_controller_write;
  impl->input.read = standard_controller_read;
  impl->state = 0;
  impl->current = 0;
  impl->strobe = false;
  return impl;
}

void input_standard_controller_update(StandardController *self, uint8_t state) {
  self->state = state;
}

void standard_controller_write(Input *self, uint8_t state) {
  StandardController *impl = (StandardController *)self;

  impl->strobe = (state & 1) == 1;
  impl->current = 1;
}

uint8_t standard_controller_read(Input *self) {
  StandardController *impl = (StandardController *)self;

  uint8_t value = 0;
  if (impl->strobe) {
    value = impl->state & STANDARD_A;
  } else {
    uint8_t input = impl->state & impl->current;
    impl->current <<= 1;
    if (0 < input) {
      value = 1;
    }
  }
  return value | 0x40;
}
