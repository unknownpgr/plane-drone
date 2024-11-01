#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "protocol-serial.hpp"

#define PIN_NRF_CE 7
#define PIN_NRF_CSN 8

RF24 radio(PIN_NRF_CE, PIN_NRF_CSN);

const byte address[6] = "00001";

void setup()
{
  Serial.begin(9600);
  Serial.println("Controller started.");
  Serial.println("Sleep 3 seconds before enable radio.");
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.enableAckPayload();
  Serial.println("Radio enabled.");
  Serial.println("Controller ready.");
  Serial.write(0x80);

  SerialState serialState;
  serialState.state = STATE_IDLE;
  uint8_t data[10];

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
      }
    }
  }
}