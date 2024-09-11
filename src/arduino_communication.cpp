#include "core/communication.hpp"
#include "arduino_pinmap.hpp"

#include <SPI.h>
#include "RF24.h"
#include "nRF24L01.h"

const RF24 radio(PIN_NRF_CE, PIN_NRF_CSN);
const byte address[6] = "00001";

static uint8_t radio_mode;

void radio_init(uint8_t mode)
{
  radio_mode = mode;

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);

  if (mode == RADIO_MODE_RECEIVE)
  {
    radio.openReadingPipe(1, address);
    radio.startListening();
  }
  else
  {
    radio.openWritingPipe(address);
    radio.stopListening();
  }
}

bool radio_receive(uint8_t *data)
{
  if (radio_mode != RADIO_MODE_RECEIVE)
    return false;
  if (!radio.available())
    return false;
  radio.read(data, sizeof(data));
  return true;
}

void radio_send(uint8_t *data)
{
  if (radio_mode != RADIO_MODE_TRANSMIT)
    return;
  radio.write(data, sizeof(data));
}

void serial_init()
{
  Serial.begin(9600);
}

int serial_read(uint8_t *data, int len)
{
  return Serial.readBytes(data, len);
}

void serial_write(uint8_t *data, int len)
{
  Serial.write(data, len);
}