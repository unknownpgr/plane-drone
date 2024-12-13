#pragma once

#include <stdint.h>
void nrf24_init(void);
int nrf24_receive(uint8_t *buf, uint8_t len);
