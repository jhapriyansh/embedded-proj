#include "scheduler.h"

#define MAX_TASKS 8

struct Task {
  bool active;
  uint32_t next;
  task_fn fn;
};

static Task tasks[MAX_TASKS];

void scheduler_init() {
  for (int i = 0; i < MAX_TASKS; i++) {
    tasks[i].active = false;
    tasks[i].fn = nullptr;
    tasks[i].next = 0;
  }
}

int scheduler_add(task_fn fn) {
  for (int i = 0; i < MAX_TASKS; i++) {
    if (tasks[i].fn == nullptr) {
      tasks[i].fn = fn;
      return i;
    }
  }
  return -1;
}

void scheduler_enable(int id, bool en) {
  if (id < 0 || id >= MAX_TASKS) return;
  tasks[id].active = en;
}

void scheduler_set_next(int id, uint32_t next) {
  if (id < 0 || id >= MAX_TASKS) return;
  tasks[id].next = next;
}

void scheduler_tick(uint32_t now) {
  for (int i = 0; i < MAX_TASKS; i++) {
    if (!tasks[i].active || tasks[i].fn == nullptr) continue;
    if (now >= tasks[i].next) {
      tasks[i].fn(now);
    }
  }
}
