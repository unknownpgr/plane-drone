#include <core/communication.h>
#include <stdio.h>
#include <pico/stdlib.h>

void radio_init(uint8_t mode)
{
}

bool radio_receive(uint8_t *data)
{
  return false;
}

void radio_send(uint8_t *data)
{
}

void serial_init()
{
  stdio_init_all();
}

void serial_write(uint8_t *data, int len)
{
  for (int i = 0; i < len; i++)
  {
    putchar(data[i]);
  }
}

int serial_read(uint8_t *data, int len)
{
  for (int i = 0; i < len; i++)
  {
    data[i] = getchar();
  }
  return len;
}
