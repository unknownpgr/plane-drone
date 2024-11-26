#include "core/spi.h"
#include <avr/io.h>

/**
 * CE   : PB4
 * SS   : PB0
 * SCK  : PB1
 * MOSI : PB2
 * MISO : PB3
 */

void spi_init()
{
    DDRB |= (1 << DDB2) | (1 << DDB1) | (1 << DDB0); // Set MOSI, SCK, SS as output
    DDRB |= (1 << DDB4);                             // Set CE as output
    DDRB &= ~(1 << DDB3);                            // Set MISO as input

    SPCR = (1 << SPE) | (1 << MSTR); // Enable SPI, Master
    SPSR = (1 << SPI2X);             // SPI 2x speed

    spi_csn(HIGH);
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

void spi_ce(bool state)
{
    if (state)
    {
        PORTB |= (1 << PB4);
    }
    else
    {
        PORTB &= ~(1 << PB4);
    }
}

void spi_csn(bool state)
{
    if (state)
    {
        PORTB |= (1 << PB0);
    }
    else
    {
        PORTB &= ~(1 << PB0);
    }
}