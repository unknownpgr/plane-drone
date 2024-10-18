#include "core-imu.hpp"
#include <stdint.h>
#include <MPU6050.h>
#include <Wire.h>

const MPU6050 mpu;

void imu_init()
{
  Wire.begin();
  mpu.initialize();
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_500);
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_4);
}

void imu_read(IMUData *data)
{
  uint16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  data->ax = ax / 32768.0 * 4.0 * 9.8;
  data->ay = ay / 32768.0 * 4.0 * 9.8;
  data->az = az / 32768.0 * 4.0 * 9.8;
  data->gx = gx / 32768.0 * 500.0 / 180.0 * M_PI;
  data->gy = gy / 32768.0 * 500.0 / 180.0 * M_PI;
  data->gz = gz / 32768.0 * 500.0 / 180.0 * M_PI;
}