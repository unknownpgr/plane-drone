#pragma once

#include <stdint.h>
#include <stdbool.h>

void uart_init(uint32_t baudrate);
bool uart_available();
void uart_write(uint8_t *data, uint32_t len);
void uart_read(uint8_t *data, uint32_t len);