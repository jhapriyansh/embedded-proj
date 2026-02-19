#include "devices_led.h"
#include "gpio_manager.h"
#include "scheduler.h"
#include <string.h>

#define MAX_LED 4
#define LEDC_FREQ 5000
#define LEDC_RES 8

#define DOT 150
#define DASH (3 * DOT)
#define GAP DOT
#define LETTER_GAP (3 * DOT)
#define WORD_GAP (7 * DOT)
#define POST_MORSE_GAP 1000

enum Mode { IDLE, BLINK, MORSE };

struct MorseStep {
  bool on;
  uint16_t dur;
};

struct Led {
  bool bound;
  uint8_t pin;
  bool state;
  uint8_t brightness;
  Mode mode;
  uint32_t next;
  bool level;

  MorseStep seq[200];
  uint8_t len;
  uint8_t idx;
  bool post_gap;
};

static Led leds[MAX_LED];
static int task_id;

static const char* morse_for(char c) {
  static const struct { char c; const char* m; } map[] = {
    {'a',".-"},{'b',"-..."},{'c',"-.-."},{'d',"-.."},
    {'e',"."},{'f',"..-."},{'g',"--."},{'h',"...."},
    {'i',".."},{'j',".---"},{'k',"-.-"},{'l',".-.."},
    {'m',"--"},{'n',"-."},{'o',"---"},{'p',".--."},
    {'q',"--.-"},{'r',".-."},{'s',"..."},{'t',"-"},
    {'u',"..-"},{'v',"...-"},{'w',".--"},{'x',"-..-"},
    {'y',"-.--"},{'z',"--.."}
  };
  for (auto &e : map) if (e.c == c) return e.m;
  return nullptr;
}

static void led_tick(uint32_t now) {
  for (int i = 0; i < MAX_LED; i++) {
    Led &L = leds[i];
    if (!L.bound || !L.state) continue;

    if (L.mode == BLINK && now >= L.next) {
      L.level = !L.level;
      ledcWrite(L.pin, L.level ? L.brightness : 0);
      L.next = now + 500;
    }

    if (L.mode == MORSE && now >= L.next) {
      auto &s = L.seq[L.idx++];
      ledcWrite(L.pin, s.on ? L.brightness : 0);
      L.next = now + s.dur;

      if (L.idx >= L.len) {
        L.mode = IDLE;
        L.post_gap = true;
        ledcWrite(L.pin, 0);
        L.next = now + POST_MORSE_GAP;
      }
    }

    if (L.post_gap && now >= L.next) {
      L.post_gap = false;
      ledcWrite(L.pin, L.brightness);
    }
  }
  scheduler_set_next(task_id, now + 5);
}

void led_init() {
  memset(leds, 0, sizeof(leds));
  task_id = scheduler_add(led_tick);
  scheduler_enable(task_id, true);
}

bool led_is_bound(uint8_t id) {
  return id < MAX_LED && leds[id].bound;
}

bool led_bind(uint8_t id, uint8_t pin) {
  if (id >= MAX_LED) return false;
  if (leds[id].bound) return false;
  if (!gpio_claim(pin)) return false;

  Led &L = leds[id];
  memset(&L, 0, sizeof(Led));
  L.bound = true;
  L.pin = pin;
  L.brightness = 255;

  ledcAttach(pin, LEDC_FREQ, LEDC_RES);
  ledcWrite(pin, 0);
  return true;
}

void led_unbind(uint8_t id) {
  if (!led_is_bound(id)) return;
  gpio_release(leds[id].pin);
  ledcDetach(leds[id].pin);
  memset(&leds[id], 0, sizeof(Led));
}

void led_state(uint8_t id, bool on) {
  if (!led_is_bound(id)) return;
  Led &L = leds[id];
  L.state = on;
  if (!on) {
    ledcWrite(L.pin, 0);
  } else {
    ledcWrite(L.pin, L.brightness);
    L.next = millis();
  }
}

void led_blink(uint8_t id, bool en) {
  if (!led_is_bound(id)) return;
  leds[id].mode = en ? BLINK : IDLE;
  if (!en && leds[id].state) {
    ledcWrite(leds[id].pin, leds[id].brightness);
  }
}

void led_brightness(uint8_t id, uint8_t pct) {
  if (!led_is_bound(id)) return;
  leds[id].brightness = map(constrain(pct,0,100),0,100,0,255);
  if (leds[id].state && leds[id].mode == IDLE)
    ledcWrite(leds[id].pin, leds[id].brightness);
}

void led_morse_pulse(uint8_t id, const char* text) {
  if (!led_is_bound(id)) return;

  Led &L = leds[id];
  L.mode = MORSE;
  L.len = 0;
  L.idx = 0;
  L.post_gap = false;

  L.seq[L.len++] = {false, LETTER_GAP};

  for (int i = 0; text[i]; i++) {
    if (text[i] == ' ') {
      L.seq[L.len++] = {false, WORD_GAP};
      continue;
    }
    const char* m = morse_for(text[i]);
    if (!m) return;
    for (int k = 0; m[k]; k++) {
      L.seq[L.len++] = {true, m[k]=='.'?DOT:DASH};
      L.seq[L.len++] = {false, GAP};
    }
    L.seq[L.len++] = {false, LETTER_GAP};
  }

  if (L.state) L.next = millis();
}
