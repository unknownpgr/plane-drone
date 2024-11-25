#pragma once

#include <stdint.h>

void uart_init(uint32_t baudrate);
void uart_write(uint8_t *data, uint32_t len);
void uart_read(uint8_t *data, uint32_t len);