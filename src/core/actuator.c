#include "core/actuator.h"
#include "core/timer.h"
#include "core/pwm.h"
#include <avr/io.h>

#define CHANNEL_BLDC 1
#define CHANNEL_FLAP_LEFT 0
#define CHANNEL_FLAP_RIGHT 2

void actuator_init()
{
    pwm_init();
    actuator_setThrottle(1.f);
    timer_sleep(2000);
    actuator_setThrottle(0.f);
}

void actuator_setThrottle(float value)
{
    pwm_set(CHANNEL_BLDC, value);
}

void actuator_setFlapLeft(float angle)
{
    float value = (angle + 90.f) / 180.f;
    if (value > 1.f)
        value = 1.f;
    if (value < 0.f)
        value = 0.f;
    pwm_set(CHANNEL_FLAP_LEFT, value);
}

void actuator_setFlapRight(float angle)
{
    float value = (angle + 90.f) / 180.f;
    if (value > 1.f)
        value = 1.f;
    if (value < 0.f)
        value = 0.f;
    pwm_set(CHANNEL_FLAP_RIGHT, value);
}