#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "core/actuator.h"
#include "core/communication.h"
#include "core/imu.h"
#include "core/pose.h"
#include "core/timer.h"

#define PI 3.14159265358979323846
#define printf communication_send_serial

void l(const char *format, ...)
{
  static uint32_t timestamp = 0;
  uint16_t currentTime = timer_micros();
  timestamp += (uint16_t)(currentTime - timestamp);
  printf("[%u] ", (timestamp / 1000));
  va_list args;
  va_start(args, format);
  printf(format, args);
  va_end(args);
  printf("\r\n");
}

void mode_flight()
{

  State state;
  IMUData imuData;
  EulerAngle omega; // Unit: rad/s
  EulerAngle accel; // Unit: m/s^2
  uint16_t lastTime = timer_micros();
  RadioProtocol packet;

  float commandPitch = 0;
  float commandRoll = 0;

  initializeState(&state);

  while (true)
  {
    // Read command
    if (communication_receive_radio(&packet))
    {
      switch (packet.command)
      {
      case COMMAND_PING:
        communication_send_ack((uint8_t *)"PING", 4);
        break;
      case COMMAND_THROTTLE:
        communication_send_ack((uint8_t *)"THROTTLE", 8);
        actuator_setThrottle((float)(packet.throttle.throttle / 65535.f));
        break;
      case COMMAND_PITCH:
        communication_send_ack((uint8_t *)"PITCH", 5);
        commandPitch = (float)(packet.pitch.pitch * (90.f / 32767.f)) - 90.f;
        break;
      case COMMAND_ROLL:
        communication_send_ack((uint8_t *)"ROLL", 4);
        commandRoll = (float)(packet.roll.roll * (90.f / 32767.f)) - 90.0f;
        break;
      default:
        communication_send_ack((uint8_t *)"UNKNOWN", 7);
        break;
      }
    }

    imu_read(&imuData);

    // 센서가 위아래로 뒤집힌 경우, y, z축만 반대로 뒤집힌다. x축을 기준으로 rotation했기 때문이다.
    // 이때 피치는 일반적인 경우와 다르게 기수가 아래로 가면 양수임에 유의하라. 좌표계를 그렇게 설정했기 때문이다.

    omega.roll = imuData.gx;
    omega.pitch = -imuData.gy;
    omega.yaw = -imuData.gz;
    accel.roll = imuData.ax;
    accel.pitch = -imuData.ay;
    accel.yaw = -imuData.az;

    uint16_t currentTime = timer_micros();
    float dt = (currentTime - lastTime) / 1000000.0;
    lastTime = currentTime;

    estimateByComplementaryFilter(&state, &omega, &accel, dt, &state);

    float flapLeft = (state.pitch + state.roll) * 180.0 / PI - commandPitch - commandRoll;
    float flapRight = (state.pitch - state.roll) * 180.0 / PI - commandPitch + commandRoll;

    flapLeft = flapLeft > 60 ? 60 : flapLeft;
    flapLeft = flapLeft < -60 ? -60 : flapLeft;
    flapRight = flapRight > 60 ? 60 : flapRight;
    flapRight = flapRight < -60 ? -60 : flapRight;

    actuator_setFlapLeft(flapLeft);
    actuator_setFlapRight(flapRight);
  }
}

void mode_test()
{
  // Print IMU data
  IMUData imuData;
  while (true)
  {
    imu_read(&imuData);
    l("ax: %f, ay: %f, az: %f, gx: %f, gy: %f, gz: %f", imuData.ax, imuData.ay, imuData.az, imuData.gx, imuData.gy, imuData.gz);
  }
}

int main()
{
  // Initialization
  communication_init();
  actuator_init();
  imu_init();
  timer_init();

  char buffer[64];
  communication_receive_serial(buffer, 4);

  if (strcmp(buffer, "FLYT") == 0)
  {
    mode_flight();
  }

  if (strcmp(buffer, "TEST") == 0)
  {
    mode_test();
  }

  while (true)
    ;
  return 0;
}
