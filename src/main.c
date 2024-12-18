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
  // State estimation variables
  State state;
  IMUData imuData;
  uint16_t lastTime = timer_micros();
  ControllerData control;
  initializeState(&state);

  // Control command variables
  float commandThrottle = 0; // 0 ~ 1
  float commandPitch = 0;    // -1 ~ 1
  float commandRoll = 0;     // -1 ~ 1

  // PD control variables
  float gainP = 0.75f;
  float gainD = 0.2f;
  float previousPitchError = 0;
  float previousRollError = 0;
  float maxFlapAngle = 75;

  // Deadman switch
  uint16_t deadmanSwitchDefault = 600;
  uint16_t deadmanSwitch = deadmanSwitchDefault;

  while (true)
  {
    // Check deadman switch (controller connection)
    {
      if (deadmanSwitch > 0)
      {
        deadmanSwitch--;
      }
      else
      {
        // If deadman switch is activated, disable throttle.
        // Flap control is still required for safe landing.
        actuator_setThrottle(0);
        util_set_led(false);
        commandThrottle = 0;
      }
    }

    // Read command from the controller if available
    if (communication_receive_radio(&control))
    {
      deadmanSwitch = deadmanSwitchDefault;
      util_set_led(true);

      // NOTE: range of control values are 0 ~ 1023
      uint16_t throttleRaw = control.throttle;
      uint16_t pitchRaw = control.pitch;
      uint16_t rollRaw = control.roll;

      commandThrottle = throttleRaw / 1023.f;

      float newPitchCommand = (pitchRaw - 512.f) / 512.f;
      float newRollCommand = (rollRaw - 512.f) / 512.f;

      // Use low-pass filter (IIR) to smooth the command
      commandPitch = 0.1 * commandPitch + 0.9 * newPitchCommand;
      commandRoll = 0.1 * commandRoll + 0.9 * newRollCommand;
    }

    imu_read(&imuData);

    // 센서가 위아래로 뒤집힌 경우, y, z축만 반대로 뒤집힌다. x축을 기준으로 rotation했기 때문이다.
    // 이때 피치는 일반적인 경우와 다르게 기수가 아래로 가면 양수임에 유의하라. 좌표계를 그렇게 설정했기 때문이다.

    EulerAngle omega; // Unit: rad/s
    EulerAngle accel; // Unit: m/s^2
    float dt;         // Unit: s
    {

      omega.roll = imuData.gx;
      omega.pitch = -imuData.gy;
      omega.yaw = -imuData.gz;
      accel.roll = imuData.ax;
      accel.pitch = -imuData.ay;
      accel.yaw = -imuData.az;
      uint16_t currentTime = timer_micros();
      dt = (currentTime - lastTime) / 1000000.0;
      lastTime = currentTime;
      estimateByComplementaryFilter(&state, &omega, &accel, dt, &state);
    }

    // Control pitch and roll using PD control

    float pitchError = commandPitch - state.pitch;
    float rollError = commandRoll - state.roll;

    float pitchP = gainP * pitchError;
    float rollP = gainP * rollError;

    float pitchD = gainD * (pitchError - previousPitchError) / dt;
    float rollD = gainD * (rollError - previousRollError) / dt;
    previousPitchError = pitchError;
    previousRollError = rollError;

    float pitchControl = pitchP + pitchD;
    float rollControl = rollP + rollD;

    float flapLeft = (pitchControl + rollControl) * 90.0;
    float flapRight = (pitchControl - rollControl) * 90.0;

    // Limit the flap angle

    flapLeft = flapLeft > maxFlapAngle ? maxFlapAngle : flapLeft;
    flapLeft = flapLeft < -maxFlapAngle ? -maxFlapAngle : flapLeft;
    flapRight = flapRight > maxFlapAngle ? maxFlapAngle : flapRight;
    flapRight = flapRight < -maxFlapAngle ? -maxFlapAngle : flapRight;

    // Set the actuator values

    actuator_setThrottle(commandThrottle);
    actuator_setFlapLeft(-flapLeft);
    actuator_setFlapRight(+flapRight);
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
