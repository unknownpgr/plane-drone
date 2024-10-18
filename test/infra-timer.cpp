#include "core-timer.hpp"
#include <Arduino.h>

void timer_init()
{
  // Initialize timer
}

uint16_t timer_micros()
{
  return micros();
}

void timer_sleep(uint16_t ms)
{
  delay(ms);
}