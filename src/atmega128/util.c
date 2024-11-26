#include "core/util.h"
#include "core/timer.h"
#include <avr/io.h>

void util_init()
{
    DDRA = 0xFF;
}

void util_set_led(bool on)
{
    if (on)
    {
        PORTA = 0xAA; // 0b10101010
    }
    else
    {
        PORTA = 0x00;
    }
}

/**
 * Check if the jumper(pin2 is connected to ground)
 */
bool util_check_jumper()
{
}

/**
 * Delay for a given number of milliseconds
 */
void util_delay_ms(uint32_t ms)
{
    timer_sleep(ms);
}