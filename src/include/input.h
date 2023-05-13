#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

// https://www.nesdev.org/wiki/Input_devices
typedef struct input {
  void (*write)(struct input *, uint8_t state);
  uint8_t (*read)(struct input *);
} Input;

void input_write(Input *input, uint8_t state);

uint8_t input_read(Input *input);

// http://wiki.nesdev.com/w/index.php/Standard_controller
typedef struct {
  Input input;

  uint8_t state;
  uint8_t current;
  bool strobe;
} StandardController;

StandardController *input_standard_controller_new();

typedef enum {
  STANDARD_A = 1 << 0,
  STANDARD_B = 1 << 1,
  STANDARD_SELECT = 1 << 2,
  STANDARD_START = 1 << 3,
  STANDARD_UP = 1 << 4,
  STANDARD_DOWN = 1 << 5,
  STANDARD_LEFT = 1 << 6,
  STANDARD_RIGHT = 1 << 7,
} StandardControllerButton;

void input_standard_controller_update(StandardController *self, uint8_t state);

#endif // INPUT_H
