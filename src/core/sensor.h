#pragma once

#include <stdint.h>
#include <stdbool.h>

void sensor_init();

bool sensor_read_imu(uint16_t *ax, uint16_t *ay, uint16_t *az, uint16_t *gx, uint16_t *gy, uint16_t *gz);