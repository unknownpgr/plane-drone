#include <core/actuator.h>
#include <pico/stdlib.h>

#define PIN_SERVO_LEFT 7
#define PIN_SERVO_RIGHT 8
#define PIN_BLDC 9

void actuator_init()
{
  // Initialize servos
  gpio_init(PIN_SERVO_LEFT);
  gpio_set_function(PIN_SERVO_LEFT, GPIO_FUNC_PWM);
  gpio_set_dir(PIN_SERVO_LEFT, GPIO_OUT);
  gpio_put(PIN_SERVO_LEFT, 0);

  gpio_init(PIN_SERVO_RIGHT);
  gpio_set_function(PIN_SERVO_RIGHT, GPIO_FUNC_PWM);
  gpio_set_dir(PIN_SERVO_RIGHT, GPIO_OUT);
  gpio_put(PIN_SERVO_RIGHT, 0);

  // Initialize BLDC
  gpio_init(PIN_BLDC);
  gpio_set_function(PIN_BLDC, GPIO_FUNC_PWM);
  gpio_set_dir(PIN_BLDC, GPIO_OUT);
  gpio_put(PIN_BLDC, 0);

  // Calibrate BLDC
  gpio_put(PIN_BLDC, 1);
  sleep_ms(2000);
  gpio_put(PIN_BLDC, 0);
  sleep_ms(2000);
}

void actuator_set_bldc(uint8_t speed)
{
  gpio_put(PIN_BLDC, speed);
}

void actuator_set_servo_left(uint8_t angle)
{
  gpio_put(PIN_SERVO_LEFT, angle);
}

void actuator_set_servo_right(uint8_t angle)
{
  gpio_put(PIN_SERVO_RIGHT, angle);
}