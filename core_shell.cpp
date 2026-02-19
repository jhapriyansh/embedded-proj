#include "core_shell.h"
#include "devices_led.h"
#include "logger.h"
#include <Arduino.h>
#include <string.h>
#include <stdlib.h>

static char buf[128];
static uint8_t idx = 0;

static int tokenize(char *s, char *argv[], int max) {
  int c = 0;
  char *t = strtok(s, " ");
  while (t && c < max) {
    argv[c++] = t;
    t = strtok(nullptr, " ");
  }
  return c;
}

void shell_init() {
  idx = 0;
}

static void shell_exec(char *line) {
  Serial.print("> ");
  Serial.println(line);

  char *argv[8];
  int argc = tokenize(line, argv, 8);

  if (argc == 0) { Serial.println("ERR"); return; }

  if (argc == 2 && strcmp(argv[0], "log") == 0 && strcmp(argv[1], "--show") == 0) {
    log_dump();
    Serial.println("OK");
    return;
  }

  if (strcmp(argv[0], "led") == 0) {
    if (argc < 3) { Serial.println("ERR"); return; }

    uint8_t id = atoi(argv[1]);

    if (argc == 4 && strcmp(argv[2], "-s") == 0) {
      int pin = atoi(argv[3]);
      if (!led_bind(id, pin)) { Serial.println("ERR"); return; }
      Serial.println("OK");
      return;
    }

    if (!led_is_bound(id)) { Serial.println("ERR"); return; }

    if (argc == 3 && strncmp(argv[2], "--state=", 8) == 0) {
      led_state(id, atoi(argv[2]+8));
      Serial.println("OK"); return;
    }

    if (argc == 3 && strncmp(argv[2], "--blink=", 8) == 0) {
      led_blink(id, atoi(argv[2]+8));
      Serial.println("OK"); return;
    }

    if (argc == 3 && strncmp(argv[2], "--brightness=", 13) == 0) {
      led_brightness(id, atoi(argv[2]+13));
      Serial.println("OK"); return;
    }

    if (argc == 3 && strncmp(argv[2], "--morsePulse=", 13) == 0) {
      char *p = argv[2] + 13;
      if (*p != '\'' || p[strlen(p)-1] != '\'') { Serial.println("ERR"); return; }
      p[strlen(p)-1] = 0;
      led_morse_pulse(id, p+1);
      Serial.println("OK"); return;
    }

    if (argc == 3 && strcmp(argv[2], "--unbind") == 0) {
      led_unbind(id);
      Serial.println("OK"); return;
    }

    Serial.println("ERR");
    return;
  }

  Serial.println("ERR");
}

void shell_tick() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      buf[idx] = 0;
      shell_exec(buf);
      idx = 0;
    } else if (idx < sizeof(buf)-1) {
      buf[idx++] = c;
    }
  }
}
