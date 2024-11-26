#include "core/i2c.h"
#include <avr/io.h>

#define F_CPU 16000000UL // MCU 클럭 속도
#define F_SCL 100000UL   // I2C 클럭 속도
#define TWBR_VAL (((F_CPU / F_SCL) - 16) / 2)

void i2c_init(void)
{
    TWSR = 0x00;
    TWBR = TWBR_VAL;
    TWCR = (1 << TWEN);
}

void i2c_start(uint8_t address)
{
    TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
        ;
    TWDR = address;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
        ;
}

void i2c_stop(void)
{
    TWCR = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
    while (TWCR & (1 << TWSTO))
        ;
}

void i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
        ;
}

uint8_t i2c_read_ack(void)
{
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}

uint8_t i2c_read_nack(void)
{
    TWCR = (1 << TWEN) | (1 << TWINT);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}
