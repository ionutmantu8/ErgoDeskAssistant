#pragma once
#include <Arduino.h>

struct SensorSnapshot {
  float pitchDeg = 0.0f;          // current pitch angle computed from MPU6050
  float postureDeltaDeg = 0.0f;   // deviation from calibrated posture
  float distanceCm = -1.0f;       // measured distance from ultrasonic sensor
  bool  distanceValid = false;    // tells whether the distance reading is valid
  uint16_t ldrRaw = 0;            // raw ADC value from the photoresistor
};

struct AlertState {
  bool posture = false;           // posture alert active
  bool distance = false;          // distance alert active
  bool postureCritical = false;   // posture alert reached critical threshold
  bool distanceCritical = false;  // distance alert reached critical threshold
  bool both() const { return posture && distance; } // true if both alerts are active
  bool any()  const { return posture || distance; } // true if at least one alert is active
}; 

struct AppState {
  SensorSnapshot sensors;         // latest sensor values
  AlertState alerts;              // current alert states
  float baselinePitchDeg = 0.0f;  // calibrated reference posture
  bool calibrating = false;       // true while calibration is running
  bool alertModeOnly = true;      // LCD mode: alert screen only or detailed values
};