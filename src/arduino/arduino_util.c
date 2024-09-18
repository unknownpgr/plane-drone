#include "core/util.hpp"
#include <Arduino.h>

void util_init()
{
  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Jumper
  pinMode(2, INPUT_PULLUP);
}

void util_set_led(bool on)
{
  digitalWrite(LED_BUILTIN, on ? HIGH : LOW);
}

bool util_check_jumper()
{
  return digitalRead(2) == LOW;
}

void util_delay(uint32_t ms)
{
  delay(ms);
}