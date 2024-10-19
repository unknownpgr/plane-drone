#pragma once

#include <stdint.h>

#pragma pack(push, 1)
#define type typedef struct

#define COMMAND_PING 0x01
type
{
  uint32_t timestamp;
}
PingCommand;

#define COMMAND_THROTTLE 0x02
type
{
  int16_t throttle;
}
SetThrottleCommand;

#define COMMAND_PITCH 0x03
type
{
  int16_t pitch;
}
SetPitchCommand;

#define COMMAND_ROLL 0x04
type
{
  int16_t roll;
}
SetRollCommand;

type
{
  uint8_t command;
  union
  {
    PingCommand ping;
    SetThrottleCommand throttle;
    SetPitchCommand pitch;
    SetRollCommand roll;
    uint8_t bytes[30];
  };
  uint8_t checksum;
}
RadioProtocol;

// This is to ensure that the size of RadioProtocol is 32 bytes
static_assert(sizeof(RadioProtocol) == 32, "RadioProtocol size is not 32 bytes");

#undef type
#pragma pack(pop)

typedef union
{
  RadioProtocol protocol;
  uint8_t bytes[32];
} RadioProtocolPacket;