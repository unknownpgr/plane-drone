#include <core/sensor.h>
#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <hardware/gpio.h>
#include <pico/binary_info.h>

static void init_mpu6050();

void sensor_init()
{
  init_mpu6050();
}

// =======[ MPU6050 ]===========================================================

#define MPU6050_STATE_IDLE 0
#define MPU6050_STATE_READING 1

static const uint8_t MPU6050_ADDR = 0x68;

static void init_mpu6050()
{
  // Initialize MPU6050
  // SDA -> GP0
  // SCL -> GP1
  gpio_set_function(0, GPIO_FUNC_I2C);
  gpio_set_function(1, GPIO_FUNC_I2C);
  gpio_pull_up(0);
  gpio_pull_up(1);
  i2c_init(i2c0, 100 * 1000); // 100 kHz
  bi_decl(bi_2pins_with_func(0, 1, GPIO_FUNC_I2C));

  // Reset MPU6050
  i2c_write_blocking(i2c0, MPU6050_ADDR, (uint8_t[]){0x6B, 0x00}, 2, false);
  sleep_ms(100);
  // Wake up MPU6050
  i2c_write_blocking(i2c0, MPU6050_ADDR, (uint8_t[]){0x6B, 0x80}, 2, false);
  sleep_ms(100);
}

bool sensor_read_imu(uint16_t *ax, uint16_t *ay, uint16_t *az, uint16_t *gx, uint16_t *gy, uint16_t *gz)
{
  // Read raw data from MPU6050
  uint8_t val = 0x3B;
  uint8_t buffer[14];
  i2c_write_blocking(i2c0, MPU6050_ADDR, &val, 1, true);
  i2c_read_blocking(i2c0, MPU6050_ADDR, buffer, 14, false);

  // Parse raw data
  *ax = (buffer[0] << 8) | buffer[1];
  *ay = (buffer[2] << 8) | buffer[3];
  *az = (buffer[4] << 8) | buffer[5];
  // Skip temperature
  *gx = (buffer[8] << 8) | buffer[9];
  *gy = (buffer[10] << 8) | buffer[11];
  *gz = (buffer[12] << 8) | buffer[13];

  return true;
}