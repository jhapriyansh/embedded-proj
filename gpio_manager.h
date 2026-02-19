#pragma once
#include <Arduino.h>

#define MAX_GPIO 40

void gpio_init();

bool gpio_claim(uint8_t pin);
void gpio_release(uint8_t pin);
bool gpio_is_claimed(uint8_t pin);
