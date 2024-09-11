#pragma once

#include <stdint.h>

void actuator_init();

void actuator_set_bldc(uint8_t speed);
void actuator_set_servo_left(uint8_t angle);
void actuator_set_servo_right(uint8_t angle);