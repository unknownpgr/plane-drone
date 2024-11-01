#include "protocol-serial.hpp"

bool protocol_serial_read(SerialState *state, uint8_t inputData)
{
  switch (state->state)
  {
  case STATE_IDLE:
    if (inputData == MAGIC_BYTE_1)
    {
      state->state = STATE_READY_1;
    }
    break;

  case STATE_READY_1:
    if (inputData == MAGIC_BYTE_2)
    {
      state->state = STATE_READY_2;
    }
    else
    {
      state->state = STATE_IDLE;
    }
    break;

  case STATE_READY_2:
    state->index = 0;
    state->state = STATE_READ_DATA;
    state->data[state->index++] = inputData;
    break;

  case STATE_READ_DATA:
    state->data[state->index++] = inputData;
    if (state->index == 32)
    {
      state->state = STATE_IDLE;
      return true;
    }
    break;

  default:
    state->state = STATE_IDLE;
    break;
  }

  return false;
}
