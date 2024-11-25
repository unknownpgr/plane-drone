#include "core/actuator.h"
#include "core/timer.h"
#include <avr/io.h>

void actuator_init()
{
    // Set PB5, PB6, PB7 as output
    DDRB |= (1 << DDB5) | (1 << DDB6) | (1 << DDB7);

    // Set Fast PWM mode with ICR1 as top, non-inverted mode, prescaler 8
    TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    // Set top value for 50Hz PWM
    ICR1 = 39999;

    // Set initial duty cycle to 1.5ms (neutral position for servo)
    OCR1A = 1500;
    OCR1B = 1500;
    OCR1C = 1500;

    // Initialize the BLDC motor
    actuator_setThrottle(1.f);
    timer_sleep(2000);
    actuator_setThrottle(0.f);
}

void actuator_setThrottle(float value)
{
    if (value > 1.f)
        value = 1.f;
    if (value < 0.f)
        value = 0.f;
    value = 0.05f + value * 0.05f;

    // Set duty cycle from 5%~10%
    OCR1A = ICR1 * value;
}

void actuator_setFlapLeft(float angle)
{
    float value = (angle + 90.f) / 180.f;
    if (value > 1.f)
        value = 1.f;
    if (value < 0.f)
        value = 0.f;
    value = 0.05f + value * 0.05f;

    // Set duty cycle from 5%~10%
    OCR1B = ICR1 * value;
}

void actuator_setFlapRight(float angle)
{
    float value = (angle + 90.f) / 180.f;
    if (value > 1.f)
        value = 1.f;
    if (value < 0.f)
        value = 0.f;
    value = 0.05f + value * 0.05f;

    // Set duty cycle from 5%~10%
    OCR1C = ICR1 * value;
}