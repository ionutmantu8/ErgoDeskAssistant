#pragma once
#include <Arduino.h>

static const uint8_t LCD_ADDR = 0x27;
static const uint8_t MPU_ADDR = 0x68;

static const uint16_t SENSOR_PERIOD_MS     = 80;
static const uint16_t LCD_PERIOD_MS        = 250;
static const uint16_t BUTTON_DEBOUNCE_MS   = 40;
static const uint16_t LONG_PRESS_MS        = 1000;
static const uint16_t POSTURE_HOLD_MS      = 1500;
static const uint16_t DISTANCE_HOLD_MS     = 200;
static const uint16_t SEVERITY_STABLE_MS   = 400;

static const float POSTURE_WARN_DEG        = 20.0f;
static const float POSTURE_CRITICAL_DEG    = 50.0f;

static const float DISTANCE_WARN_CM        = 15.0f;
static const float DISTANCE_CRITICAL_CM    = 5.0f;

static const uint8_t CALIBRATION_SAMPLES   = 25;
static const uint16_t CALIBRATION_DELAY_MS = 40;

static const uint8_t MPU_REG_PWR_MGMT_1    = 0x6B;
static const uint8_t MPU_REG_CONFIG        = 0x1A;
static const uint8_t MPU_REG_GYRO_CONFIG   = 0x1B;
static const uint8_t MPU_REG_ACCEL_CONFIG  = 0x1C;
static const uint8_t MPU_REG_ACCEL_XOUT_H  = 0x3B;

static const uint8_t LCD_RS = 0x01;
static const uint8_t LCD_RW = 0x02;
static const uint8_t LCD_EN = 0x04;
static const uint8_t LCD_BL = 0x08;