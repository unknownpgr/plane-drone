#pragma once

#include <stdint.h>

void actuator_init();

/**
 * @brief Set the BLDC motor speed
 * min: 0, max: 1
 */
void actuator_setThrottle(float value);

/**
 * @brief Set the left flap angle
 * min: -90, max: 90
 * Note: higher value means upward flap
 */
void actuator_setFlapLeft(float angle);

/**
 * @brief Set the right flap angle
 * min: -90, max: 90
 * Note: higher value means upward flap
 */
void actuator_setFlapRight(float angle);