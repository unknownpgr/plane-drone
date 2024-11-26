#pragma once

#include <stdint.h>

void pwm_init();
void pwm_set(uint8_t channel, float value);