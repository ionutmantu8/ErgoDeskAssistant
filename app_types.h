#pragma once
#include <Arduino.h>

struct SensorSnapshot {
  float pitchDeg = 0.0f;
  float postureDeltaDeg = 0.0f;
  float distanceCm = -1.0f;
  bool  distanceValid = false;
  uint16_t ldrRaw = 0;
};

struct AlertState {
  bool posture = false;
  bool distance = false;
  bool postureCritical = false;
  bool distanceCritical = false;
  bool both() const { return posture && distance; }
  bool any()  const { return posture || distance; }
};

struct AppState {
  SensorSnapshot sensors;
  AlertState alerts;
  float baselinePitchDeg = 0.0f;
  bool calibrating = false;
  bool alertModeOnly = true;
};