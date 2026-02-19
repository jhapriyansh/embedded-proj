#include "logger.h"
#include <string.h>

#define MAX_LOGS 32

struct LogEntry {
  uint32_t ts;
  char tag[8];
  char msg[32];
};

static LogEntry logs[MAX_LOGS];
static uint8_t count = 0;

void log_init() {
  count = 0;
}

void log_info(const char* tag, const char* msg) {
  if (count >= MAX_LOGS) return;
  logs[count].ts = millis();
  strncpy(logs[count].tag, tag, sizeof(logs[count].tag) - 1);
  strncpy(logs[count].msg, msg, sizeof(logs[count].msg) - 1);
  count++;
}

void log_dump() {
  for (uint8_t i = 0; i < count; i++) {
    Serial.printf("[%lu] %s: %s\n",
      logs[i].ts,
      logs[i].tag,
      logs[i].msg
    );
  }
}
