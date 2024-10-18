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
  radio.openReadingPipe(1, address);
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