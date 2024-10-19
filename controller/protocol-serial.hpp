#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint8_t state;
  uint8_t data[32];
  uint8_t index;
} SerialState;

#define STATE_IDLE 0x01
#define STATE_READY_1 0x02
#define STATE_READY_2 0x04
#define STATE_READ_DATA 0x08

#define MAGIC_BYTE_1 0x55
#define MAGIC_BYTE_2 0xAA

bool protocol_serial_read(SerialState *state, uint8_t inputData);