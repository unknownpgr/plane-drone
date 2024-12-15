#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "core/actuator.h"
#include "core/communication.h"
#include "core/imu.h"
#include "core/pose.h"
#include "core/timer.h"
#include "core/util.h"

#define PI 3.14159265358979323846
#define printf communication_send_serial

static void mode_flight()
{

  State state;
  IMUData imuData;
  EulerAngle omega; // Unit: rad/s
  EulerAngle accel; // Unit: m/s^2
  uint16_t lastTime = timer_micros();
  ControllerData control;

  float commandPitch = 0;
  float commandRoll = 0;
  float commandThrottle = 0;

  initializeState(&state);

  uint16_t deadmanSwitchDefault = 600;
  uint16_t deadmanSwitch = deadmanSwitchDefault;

  while (true)
  {
    util_set_led(true);
    if (deadmanSwitch > 0)
    {
      deadmanSwitch--;
    }
    else
    {
      // If deadman switch is activated, stop the motor and wait for the radio signal
      actuator_setThrottle(0);
      actuator_setFlapLeft(0);
      actuator_setFlapRight(0);
      util_set_led(false);
      communication_send_serial("DEADMAN SWITCH ACTIVATED\r\n");

      while (true)
      {
        if (communication_receive_radio(&control))
        {
          deadmanSwitch = deadmanSwitchDefault;
          break;
        }
      }
    }

    // Read command
    if (communication_receive_radio(&control))
    {
      uint16_t throttle = control.throttle;
      uint16_t pitch = control.pitch;
      uint16_t roll = control.roll;

      commandThrottle = throttle / 1023.f;
      commandPitch = (pitch - 512.f) * 90.f / 512.f;
      commandRoll = (roll - 512.f) * 90.f / 512.f;
      deadmanSwitch = deadmanSwitchDefault;
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

    float flapLeft = (state.pitch + state.roll) * 180.0 / PI + commandPitch + commandRoll;
    float flapRight = (state.pitch - state.roll) * 180.0 / PI + commandPitch - commandRoll;

    flapLeft = flapLeft > 60 ? 60 : flapLeft;
    flapLeft = flapLeft < -60 ? -60 : flapLeft;
    flapRight = flapRight > 60 ? 60 : flapRight;
    flapRight = flapRight < -60 ? -60 : flapRight;

    actuator_setThrottle(commandThrottle);
    actuator_setFlapLeft(+flapLeft);
    actuator_setFlapRight(-flapRight);
  }
}

static void mode_test_imu()
{
  // Print IMU data
  IMUData imuData;
  while (true)
  {
    imu_read(&imuData);
    communication_send_serial("ax:%d,ay:%d,az:%d,gx:%d,gy:%d,gz:%d\r\n", (int)(imuData.ax * 100), (int)(imuData.ay * 100), (int)(imuData.az * 100), (int)(imuData.gx * 100), (int)(imuData.gy * 100), (int)(imuData.gz * 100));
  }
}

static void mode_test_radio_communication()
{
  ControllerData control;
  while (true)
  {
    if (!communication_receive_radio(&control))
    {
      continue;
    }

    communication_send_serial("throttle:%04d, pitch:%04d, roll:%04d, checksum:%04d / ", control.throttle, control.pitch, control.roll, control.checksum);
    uint16_t checksum = control.throttle + control.pitch + control.roll;
    if (checksum == control.checksum)
    {
      communication_send_serial("Checksum OK\r\n");
    }
    else
    {
      communication_send_serial("Checksum Error\r\n");
    }
  }
}

int main()
{
  // Initialization
  util_init();
  timer_init();
  communication_init();
  actuator_init();
  util_set_led(true);
  imu_init();

  communication_send_serial("DEVICE READY\r\n");
  mode_flight();
  // mode_test_imu();
  // mode_test_radio_communication();

  while (true)
    ;
  return 0;
}
