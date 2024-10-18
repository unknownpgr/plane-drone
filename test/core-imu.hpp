#ifndef __CORE_IMU_H__
#define __CORE_IMU_H__

typedef float IMUScalar;

/**
 * @brief IMU Data
 * Units:
 *  - Acceleration: m/s^2
 *  - Gyroscope: rad/s
 */
typedef struct IMUData
{
  IMUScalar ax, ay, az;
  IMUScalar gx, gy, gz;
} IMUData;

void imu_init();
void imu_read(IMUData *data);

#endif