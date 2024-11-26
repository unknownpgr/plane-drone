#pragma once

#include <stdint.h>
#include <stdbool.h>

#define LOW false
#define HIGH true

void spi_init();
uint8_t spi_transfer(uint8_t data);
void spi_ce(bool state);
void spi_csn(bool state);
