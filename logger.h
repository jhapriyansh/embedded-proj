#pragma once
#include <Arduino.h>

void log_init();
void log_info(const char* tag, const char* msg);
void log_dump();
