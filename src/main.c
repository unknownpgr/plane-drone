#include "pico/stdlib.h"

void pico_set_led(bool on)
{
  gpio_put(25, on);
}

int main()
{
  gpio_init(25);
  gpio_set_dir(25, 1);
  while (true)
  {
    pico_set_led(true);
    sleep_ms(500);
    pico_set_led(false);
    sleep_ms(500);
  }
  return 0;
}