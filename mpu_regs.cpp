#include <Arduino.h>
#include "config.h"
#include "twi_regs.h"
#include "mpu_regs.h"

bool mpu_init_regs() {
  if (!i2c_write_reg(MPU_ADDR, MPU_REG_PWR_MGMT_1, 0x00)) return false;
  // Write 0x00 to PWR_MGMT_1 to wake up the MPU6050 from sleep mode

  if (!i2c_write_reg(MPU_ADDR, MPU_REG_CONFIG, 0x04)) return false;
  // Write 0x04 to CONFIG to set the digital low-pass filter configuration

  if (!i2c_write_reg(MPU_ADDR, MPU_REG_GYRO_CONFIG, 0x08)) return false;
  // Write 0x08 to GYRO_CONFIG to select the gyroscope full-scale range (+/-500 deg/s)

  if (!i2c_write_reg(MPU_ADDR, MPU_REG_ACCEL_CONFIG, 0x10)) return false;
  // Write 0x10 to ACCEL_CONFIG to select the accelerometer full-scale range (+/-8g)

  return true;
}

bool mpu_read_accel_raw(int16_t& ax, int16_t& ay, int16_t& az) {
  uint8_t data[6];

  if (!i2c_read_bytes(MPU_ADDR, MPU_REG_ACCEL_XOUT_H, data, 6)) return false;
  // Read 6 consecutive bytes starting from ACCEL_XOUT_H:
  // AX high, AX low, AY high, AY low, AZ high, AZ low

  ax = (int16_t)((data[0] << 8) | data[1]);
  // Combine the high and low bytes into the 16-bit raw X-axis acceleration value

  ay = (int16_t)((data[2] << 8) | data[3]);
  // Combine the high and low bytes into the 16-bit raw Y-axis acceleration value

  az = (int16_t)((data[4] << 8) | data[5]);
  // Combine the high and low bytes into the 16-bit raw Z-axis acceleration value

  return true;
}