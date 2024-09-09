#include "core/actuator.hpp"

#include <Servo.h>
#include "pinmap.hpp"

Servo servo_left;
Servo servo_right;
Servo bldc;

void actuator_init()
{
  // Initialize BLDC
  bldc.attach(PIN_BLDC);
  bldc.write(150); // Full throttle
  delay(3000);     // Wait for 3 seconds
  bldc.write(0);   // Stop

  // Initialize servos
  servo_left.attach(PIN_SERVO_LEFT);
  servo_right.attach(PIN_SERVO_RIGHT);
}

void actuator_set_bldc(uint8_t speed)
{
  bldc.write(speed);
}

void actuator_set_servo_left(uint8_t angle)
{
  servo_left.write(angle);
}

void actuator_set_servo_right(uint8_t angle)
{
  servo_right.write(angle);
}