#pragma once

#include <stdint.h>

typedef struct
{
  uint16_t throttle;
  uint16_t pitch;
  uint16_t roll;
  uint16_t checksum;
} ControllerData;

typedef union
{
  ControllerData data;
  uint8_t bytes[32];
} ControllerDataUnion;