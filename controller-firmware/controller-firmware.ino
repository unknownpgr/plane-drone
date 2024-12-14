#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PIN_NRF_CE 7
#define PIN_NRF_CSN 8

#define PIN_THROTTLE A5
#define PIN_PITCH A4
#define PIN_ROLL A3

typedef struct
{
  uint16_t throttle;
  uint16_t pitch;
  uint16_t roll;
  uint16_t checksum;
} ControllerData;

typedef union
{
  ControllerData data;
  uint8_t bytes[32];
} ControllerDataUnion;

void setup()
{
  // Initialize serial
  Serial.begin(9600);
  Serial.println("\n\nController started.");

  // Initialize radio
  RF24 radio(PIN_NRF_CE, PIN_NRF_CSN);
  radio.begin();
  radio.powerUp();
  uint8_t address[5] = "00001";
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableAckPayload();
  radio.setChannel(76);
  uint8_t channel = radio.getChannel();
  Serial.print("Radio enabled at channel ");
  Serial.print(channel);
  Serial.println(".");

  // Initialize analog input
  pinMode(PIN_THROTTLE, INPUT);
  pinMode(PIN_PITCH, INPUT);
  pinMode(PIN_ROLL, INPUT);
  Serial.println("Analog input ready.");

  Serial.println("Controller ready.");

  while (true)
  {
    uint16_t throttle = analogRead(PIN_THROTTLE);
    uint16_t pitch = analogRead(PIN_PITCH);
    uint16_t roll = analogRead(PIN_ROLL);

    Serial.print("Throttle: ");
    Serial.print(throttle);
    Serial.print(" Pitch: ");
    Serial.print(pitch);
    Serial.print(" Roll: ");
    Serial.println(roll);

    ControllerDataUnion controllerData;
    controllerData.data.throttle = throttle;
    controllerData.data.pitch = pitch;
    controllerData.data.roll = roll;
    controllerData.data.checksum = throttle + pitch + roll;

    radio.write(controllerData.bytes, sizeof(controllerData));

    delay(100);
  }
}
