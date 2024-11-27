#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "protocol-serial.hpp"

#define PIN_NRF_CE 7
#define PIN_NRF_CSN 8

RF24 radio(PIN_NRF_CE, PIN_NRF_CSN);

uint8_t address[5] = "00001";

void setup()
{
  Serial.begin(9600);
  Serial.println("\n\nController started.");
  radio.begin();
  radio.powerUp();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableAckPayload();
  radio.setChannel(76);
  uint8_t channel = radio.getChannel();
  Serial.print("Radio enabled at channel ");
  Serial.print(channel);
  Serial.println(".");
  Serial.println("Controller ready.");

  SerialState serialState;
  serialState.state = STATE_IDLE;
  uint8_t data[10];
  uint8_t counter = 0;

  while (true)
  {
    if (Serial.available() > 0)
    {
      uint8_t inputData = Serial.read();
      if (protocol_serial_read(&serialState, inputData))
      {
        radio.write(&serialState.data, 32);
        if (radio.isAckPayloadAvailable())
        {
          radio.read(&data, 10);
          for (int i = 0; i < 10; i++)
          {
            Serial.print(data[i]);
          }
          Serial.println();
        }
        counter++;
        Serial.print("SENT:");
        Serial.println(counter);
      }
    }
  }
}