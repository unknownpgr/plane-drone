#pragma once

#include <stdint.h>
#include <stdbool.h>

#define RADIO_MODE_RECEIVE 0
#define RADIO_MODE_TRANSMIT 1

void radio_init(uint8_t mode);
bool radio_receive(uint8_t *data);
void radio_send(uint8_t *data);

void serial_init();
int serial_read(uint8_t *data, int len);
void serial_write(uint8_t *data, int len);