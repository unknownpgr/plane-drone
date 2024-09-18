#include "core/communication.hpp"
#include "core/util.hpp"
#include "core/actuator.hpp"
#include "main.hpp"

// Function prototypes
void entrypoint_plane();
void entrypoint_controller();

void main()
{
  actuator_init();
  util_init();

  if (!util_check_jumper())
  {
    // If the jumper is not connected, the device is a plane
    radio_init(RADIO_MODE_RECEIVE);
    entrypoint_plane();
  }
  else
  {
    // If the jumper is connected, the device is a controller
    radio_init(RADIO_MODE_TRANSMIT);
    serial_init();
    entrypoint_controller();
  }
}

void entrypoint_plane()
{
  uint8_t data[32];
  while (1)
  {
    if (radio_receive(data))
    {
      util_set_led(data[0]);
      actuator_set_servo_left(data[0] ? 90 : 0);
    }
  }
}

void entrypoint_controller()
{
  const uint8_t message[] = "===Controller===\n";
  serial_write((uint8_t *)message, sizeof(message));

  uint8_t data[32];
  while (1)
  {
    data[0] = 1;
    util_delay(1000);
    radio_send(data);
    util_delay(1000);
    util_set_led(true);
    serial_write((uint8_t *)"ON\n", 3);
    actuator_set_servo_left(90);

    data[0] = 0;
    util_delay(1000);
    radio_send(data);
    util_delay(1000);
    util_set_led(false);
    serial_write((uint8_t *)"OFF\n", 4);
    actuator_set_servo_left(0);
  }
}