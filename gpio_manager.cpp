#include "gpio_manager.h"

static bool claimed[MAX_GPIO];

void gpio_init() {
  for (int i = 0; i < MAX_GPIO; i++) {
    claimed[i] = false;
  }
}

bool gpio_claim(uint8_t pin) {
  if (pin >= MAX_GPIO) return false;
  if (claimed[pin]) return false;
  claimed[pin] = true;
  return true;
}

void gpio_release(uint8_t pin) {
  if (pin >= MAX_GPIO) return;
  claimed[pin] = false;
}

bool gpio_is_claimed(uint8_t pin) {
  if (pin >= MAX_GPIO) return true;
  return claimed[pin];
}
