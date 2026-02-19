#pragma once
#include <Arduino.h>

typedef void (*task_fn)(uint32_t now);

int  scheduler_add(task_fn fn);
void scheduler_enable(int id, bool en);
void scheduler_set_next(int id, uint32_t next);
void scheduler_tick(uint32_t now);
void scheduler_init();
