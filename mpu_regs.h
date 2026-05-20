#pragma once
#include <Arduino.h>

bool mpu_init_regs();
bool mpu_read_accel_raw(int16_t& ax, int16_t& ay, int16_t& az);