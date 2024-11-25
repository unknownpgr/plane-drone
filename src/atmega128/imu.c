#include "core/imu.h"
#include "core/i2c.h"
#include <stdint.h>

#define MPU6050_ADDR 0x68 // MPU6050의 I2C 주소 (기본값)
#define PWR_MGMT_1 0x6B   // 전원 관리 레지스터
#define ACCEL_XOUT_H 0x3B // 가속도 X축 데이터 레지스터 (상위 바이트)

void imu_init()
{
    i2c_init();
    i2c_start(MPU6050_ADDR << 1);
    i2c_write(PWR_MGMT_1);
    i2c_write(0);
    i2c_stop();
}

void imu_read(IMUData *data)
{
    i2c_start(MPU6050_ADDR << 1);
    i2c_write(ACCEL_XOUT_H);
    i2c_start((MPU6050_ADDR << 1) | 1);

    int16_t ax = (i2c_read_ack() << 8) | i2c_read_ack();
    int16_t ay = (i2c_read_ack() << 8) | i2c_read_ack();
    int16_t az = (i2c_read_ack() << 8) | i2c_read_ack();
    int16_t gx = (i2c_read_ack() << 8) | i2c_read_ack();
    int16_t gy = (i2c_read_ack() << 8) | i2c_read_ack();
    int16_t gz = (i2c_read_nack() << 8) | i2c_read_nack();

    data->ax = ax * 4.0 / 32768.0;
    data->ay = ay * 4.0 / 32768.0;
    data->az = az * 4.0 / 32768.0;
    data->gx = gx * 3.14159265358979323846 * 500.0 / (32768.00 * 180.0);
    data->gy = gy * 3.14159265358979323846 * 500.0 / (32768.00 * 180.0);
    data->gz = gz * 3.14159265358979323846 * 500.0 / (32768.00 * 180.0);

    i2c_stop();
}