#include <Arduino.h>
#include "core_shell.h"
#include "scheduler.h"
#include "logger.h"
#include "gpio_manager.h"
#include "devices_led.h"

void setup() {
  Serial.begin(115200);
  delay(300);

  log_init();
  gpio_init();
  scheduler_init();
  led_init();
  shell_init();

  Serial.println("READY");
}

void loop() {
  shell_tick();
  scheduler_tick(millis());
}
