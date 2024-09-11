#pragma once

#include <stdbool.h>
#include <stdint.h>

void util_init();
void util_set_led(bool on);

/**
 * Check if the jumper(pin2 is connected to ground)
 */
bool util_check_jumper();

/**
 * Delay for a given number of milliseconds
 */
void util_delay(uint32_t ms);