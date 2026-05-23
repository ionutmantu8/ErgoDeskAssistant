#pragma once
#include <Arduino.h>

static const uint8_t LCD_ADDR = 0x27;      // I2C address of the LCD backpack
static const uint8_t MPU_ADDR = 0x68;      // I2C address of the MPU6050 sensor

static const uint16_t SENSOR_PERIOD_MS     = 80;    // Time between sensor updates
static const uint16_t LCD_PERIOD_MS        = 250;   // Time between LCD refreshes
static const uint16_t BUTTON_DEBOUNCE_MS   = 40;    // Debounce time for the button
static const uint16_t LONG_PRESS_MS        = 1000;  // Hold time required for a long press
static const uint16_t POSTURE_HOLD_MS      = 1500;  // Time posture must stay bad before raising an alert
static const uint16_t DISTANCE_HOLD_MS     = 200;   // Time distance must stay bad before raising an alert
static const uint16_t SEVERITY_STABLE_MS   = 400;   // Time a new LED severity must stay stable before switching

static const float POSTURE_WARN_DEG        = 20.0f; // Posture warning threshold in degrees
static const float POSTURE_CRITICAL_DEG    = 50.0f; // Posture critical threshold in degrees

static const float DISTANCE_WARN_CM        = 15.0f; // Distance warning threshold in centimeters
static const float DISTANCE_CRITICAL_CM    = 5.0f;  // Distance critical threshold in centimeters

static const uint8_t CALIBRATION_SAMPLES   = 25;    // Number of samples used during posture calibration
static const uint16_t CALIBRATION_DELAY_MS = 40;    // Delay between calibration samples

static const uint8_t MPU_REG_PWR_MGMT_1    = 0x6B;  // MPU6050 power management register
static const uint8_t MPU_REG_CONFIG        = 0x1A;  // MPU6050 digital low-pass filter configuration register
static const uint8_t MPU_REG_GYRO_CONFIG   = 0x1B;  // MPU6050 gyroscope configuration register
static const uint8_t MPU_REG_ACCEL_CONFIG  = 0x1C;  // MPU6050 accelerometer configuration register
static const uint8_t MPU_REG_ACCEL_XOUT_H  = 0x3B;  // First accelerometer output register (used to read AX, AY, AZ)

static const uint8_t LCD_RS = 0x01;  // LCD Register Select bit on the I2C expander
static const uint8_t LCD_RW = 0x02;  // LCD Read/Write bit on the I2C expander
static const uint8_t LCD_EN = 0x04;  // LCD Enable bit on the I2C expander
static const uint8_t LCD_BL = 0x08;  // LCD Backlight control bit on the I2C expander