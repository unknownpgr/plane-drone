#include "core-actuator.hpp"
#include <Servo.h>
#include "infra-pinmap.h"

Servo servoBLDC;
Servo servoLeft;
Servo servoRight;

void actuator_init()
{
  // Attach servos
  servoBLDC.attach(PIN_BLDC, 1000, 2000); // Set BLDC motor to pin 6, with min pulse 1000 and max pulse 2000
  servoLeft.attach(PIN_SERVO_LEFT);
  servoRight.attach(PIN_SERVO_RIGHT);

  // Initialize BLDC
  servoBLDC.write(180); // Full throttle
  delay(2000);          // Wait for 2 seconds
  servoBLDC.write(0);   // Stop
  delay(2000);          // Wait for 2 seconds again
}

void actuator_setBLDC(float value)
{
  if (value < 0)
    value = 0;
  if (value > 1)
    value = 1;
  servoBLDC.write(value * 180);
}

void actuator_setFlapLeft(float angle)
{
  if (angle < -90)
    angle = -90;
  if (angle > 90)
    angle = 90;
  servoLeft.write(angle + 90);
}

void actuator_setFlapRight(float angle)
{
  if (angle < -90)
    angle = -90;
  if (angle > 90)
    angle = 90;
  servoRight.write(90 - angle);
}