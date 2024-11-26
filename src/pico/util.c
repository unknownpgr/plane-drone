#include <core/util.h>
#include <pico/stdlib.h>

#define PIN_JUMPER 28

void util_init()
{
  // LED
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

  // Jumper
  gpio_init(PIN_JUMPER);
  gpio_set_dir(PIN_JUMPER, GPIO_IN);
  gpio_pull_up(PIN_JUMPER);
}

void util_set_led(bool on)
{
  gpio_put(PICO_DEFAULT_LED_PIN, on ? 1 : 0);
}

bool util_check_jumper()
{
  return gpio_get(PIN_JUMPER) == 0;
}

void util_delay_ms(uint32_t ms)
{
  sleep_ms(ms);
}