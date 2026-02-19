#pragma once
#include <Arduino.h>

void led_init();

bool led_is_bound(uint8_t id);
bool led_bind(uint8_t id, uint8_t pin);
void led_unbind(uint8_t id);

void led_state(uint8_t id, bool on);
void led_blink(uint8_t id, bool en);
void led_brightness(uint8_t id, uint8_t pct);
void led_morse_pulse(uint8_t id, const char* text);
