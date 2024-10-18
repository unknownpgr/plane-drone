#include <stdbool.h>
#include "core-actuator.hpp"
#include "core-communication.hpp"
#include "core-imu.hpp"
#include "core-pose.hpp"
#include "core-timer.hpp"

#define M_PI 3.14159265358979323846

void entrypoint()
{
  // Initialize
  imu_init();
  actuator_init();
  timer_init();
  communication_init();

  State state;
  IMUData imuData;
  EulerAngle omega; // Unit: rad/s
  EulerAngle accel; // Unit: m/s^2
  uint16_t lastTime = timer_micros();

  initializeState(&state);

  while (true)
  {
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

    float flapLeft = state.roll * 180.0 / M_PI + state.pitch * 180.0 / M_PI;
    float flapRight = state.roll * 180.0 / M_PI + state.pitch * 180.0 / M_PI;

    flapLeft = flapLeft > 60 ? 60 : flapLeft;
    flapLeft = flapLeft < -60 ? -60 : flapLeft;
    flapRight = flapRight > 60 ? 60 : flapRight;
    flapRight = flapRight < -60 ? -60 : flapRight;

    actuator_setFlapLeft(flapLeft);
    actuator_setFlapRight(flapRight);

    // Print state
    communication_send_radio("Roll: %f ", state.roll * 180.0 / M_PI);
    communication_send_radio("Pitch: %f ", state.pitch * 180.0 / M_PI);
    communication_send_radio("Yaw: %f\n", state.yaw * 180.0 / M_PI);
  }
}
