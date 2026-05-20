#include <Arduino.h>
#include "config.h"
#include "twi_regs.h"
#include "mpu_regs.h"

bool mpu_init_regs() {
  if (!i2c_write_reg(MPU_ADDR, MPU_REG_PWR_MGMT_1, 0x00)) return false;
  if (!i2c_write_reg(MPU_ADDR, MPU_REG_CONFIG, 0x04)) return false;
  if (!i2c_write_reg(MPU_ADDR, MPU_REG_GYRO_CONFIG, 0x08)) return false;
  if (!i2c_write_reg(MPU_ADDR, MPU_REG_ACCEL_CONFIG, 0x10)) return false;
  return true;
}

bool mpu_read_accel_raw(int16_t& ax, int16_t& ay, int16_t& az) {
  uint8_t data[6];
  if (!i2c_read_bytes(MPU_ADDR, MPU_REG_ACCEL_XOUT_H, data, 6)) return false;

  ax = (int16_t)((data[0] << 8) | data[1]);
  ay = (int16_t)((data[2] << 8) | data[3]);
  az = (int16_t)((data[4] << 8) | data[5]);
  return true;
}