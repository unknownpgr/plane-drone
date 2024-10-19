#include "core-communication.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include "infra-pinmap.hpp"

const RF24 radio(PIN_NRF_CE, PIN_NRF_CSN);
const byte address[6] = "00001";

void communication_init()
{
  // Initialize Serial
  Serial.begin(9600);

  // Initialize NRF20L01
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(0, address);
  radio.startListening();
}

void communication_send_serial(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  char buffer[256];
  vsnprintf(buffer, 256, format, args);
  va_end(args);
  Serial.print(buffer);
}

void communication_send_radio(const char *format, ...)
{
}

bool communication_receive_radio(RadioProtocol *protocol)
{
  if (radio.available())
  {
    radio.read(protocol, sizeof(RadioProtocol));
    return true;
  }
  return false;
}

void communication_send_ack(uint8_t *bytes, uint8_t size)
{
  uint8_t ack[10] = {0};
  for (int i = 0; i < size; i++)
  {
    ack[i] = bytes[i];
  }
  radio.writeAckPayload(1, ack, 10); // ACK 패킷에 데이터 포함
}