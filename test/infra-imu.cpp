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
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  data->ax = ax * 4.0 / 32768.0;
  data->ay = ay * 4.0 / 32768.0;
  data->az = az * 4.0 / 32768.0;
  data->gx = gx * 3.14159265358979323846 * 500.0 / (32768.00 * 180.0);
  data->gy = gy * 3.14159265358979323846 * 500.0 / (32768.00 * 180.0);
  data->gz = gz * 3.14159265358979323846 * 500.0 / (32768.00 * 180.0);
}