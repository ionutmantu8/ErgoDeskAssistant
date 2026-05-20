#pragma once
#include <Arduino.h>
#include "app_types.h"

extern AppState app;
extern bool lastButtonReading;
extern bool stableButtonState;
extern uint32_t lastButtonDebounce;
extern uint32_t buttonPressStart;
extern bool buttonHandled;
extern uint32_t postureStartMs;
extern uint32_t distanceStartMs;
extern uint32_t lastSensorRead;
extern uint32_t lastLcdUpdate;
extern int currentSeverity;
extern int candidateSeverity;
extern uint32_t candidateSince;
extern uint32_t lastBuzzerTick;
extern bool buzzerActive;

float computePitchDeg(float ax, float ay, float az);
float readDistanceCm();
uint8_t computeLedBrightness(uint16_t ldrRaw);
int computeSeverity();

void calibratePosture();
void updateButton();
void updateSensors();
void updateAlerts();
void updateBuzzer();
void updateLeds();
void updateLCD();