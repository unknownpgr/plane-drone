#pragma once

#include <stdint.h>

void timer_init();
uint16_t timer_micros();
void timer_sleep(uint16_t ms);