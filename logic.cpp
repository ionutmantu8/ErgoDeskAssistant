#include <Arduino.h>
#include <math.h>
#include <string.h>
#include "config.h"
#include "app_types.h"
#include "gpio.h"
#include "pwm.h"
#include "adc_regs.h"
#include "lcd_regs.h"
#include "mpu_regs.h"
#include "logic.h"

AppState app;

bool lastButtonReading = true;
bool stableButtonState = true;
uint32_t lastButtonDebounce = 0;
uint32_t buttonPressStart = 0;
bool buttonHandled = false;

uint32_t postureStartMs = 0;
uint32_t distanceStartMs = 0;
uint32_t lastSensorRead = 0;
uint32_t lastLcdUpdate = 0;

int currentSeverity = 0;
int candidateSeverity = 0;
uint32_t candidateSince = 0;

uint32_t lastBuzzerTick = 0;
bool buzzerActive = false;

float computePitchDeg(float ax, float ay, float az) {
  return atan2(ax, sqrt(ay * ay + az * az)) * 180.0f / PI;
}

float readDistanceCm() {
  trig_low();
  delayMicroseconds(2);
  trig_high();
  delayMicroseconds(10);
  trig_low();

  unsigned long start = micros();
  while (!echo_read()) {
    if (micros() - start > 30000UL) return -1.0f;
  }

  unsigned long echoStart = micros();
  while (echo_read()) {
    if (micros() - echoStart > 30000UL) return -1.0f;
  }

  unsigned long duration = micros() - echoStart;
  float cm = duration * 0.0343f / 2.0f;

  if (cm < 2.0f || cm > 400.0f) return -1.0f;
  return cm;
}

static void leds_apply_severity(int severity, uint8_t brightness) {
  if (severity == 1) {
    led_set_green(brightness);
    led_set_blue(0);
    led_set_yellow(0);
  } else if (severity == 2) {
    led_set_green(0);
    led_set_blue(brightness);
    led_set_yellow(0);
  } else if (severity == 3) {
    led_set_green(0);
    led_set_blue(0);
    led_set_yellow(brightness);
  } else {
    leds_off();
  }
}

uint8_t computeLedBrightness(uint16_t ldrRaw) {
  long brightness = map(ldrRaw, 0, 1023, 15, 255);
  if (brightness < 15) brightness = 15;
  if (brightness > 255) brightness = 255;
  return (uint8_t)brightness;
}

int computeSeverity() {
  if (!app.alerts.any()) return 0;
  if (app.alerts.both()) return 3;

  if (app.alerts.posture) {
    float tilt = app.sensors.postureDeltaDeg;
    if (tilt >= 50.0f) return 3;
    if (tilt >= 35.0f) return 2;
    return 1;
  }

  if (app.alerts.distance && app.sensors.distanceValid) {
    float dist = app.sensors.distanceCm;
    if (dist <= 5.0f) return 3;
    if (dist <= 10.0f) return 2;
    return 1;
  }

  return 0;
}

void calibratePosture() {
  app.calibrating = true;
  leds_off();
  noTone(9);
  buzzerActive = false;

  float sum = 0.0f;
  uint8_t valid = 0;

  for (uint8_t i = 0; i < CALIBRATION_SAMPLES; i++) {
    lcd_show_calibration(i + 1, CALIBRATION_SAMPLES);

    int16_t axr, ayr, azr;
    if (mpu_read_accel_raw(axr, ayr, azr)) {
      float ax = (float)axr;
      float ay = (float)ayr;
      float az = (float)azr;
      float pitch = computePitchDeg(ax, ay, az);
      sum += pitch;
      valid++;
    }

    delay(CALIBRATION_DELAY_MS);
  }

  if (valid > 0) {
    app.baselinePitchDeg = sum / valid;
  }

  postureStartMs = 0;
  distanceStartMs = 0;
  app.alerts = {};
  app.calibrating = false;
}

void updateButton() {
  bool reading = button_raw_read();
  uint32_t now = millis();

  if (reading != lastButtonReading) {
    lastButtonDebounce = now;
  }

  if (now - lastButtonDebounce > BUTTON_DEBOUNCE_MS) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == LOW) {
        buttonPressStart = now;
        buttonHandled = false;
      } else {
        if (!buttonHandled) {
          app.alertModeOnly = !app.alertModeOnly;
        }
      }
    } else if (stableButtonState == LOW && !buttonHandled) {
      if (now - buttonPressStart > LONG_PRESS_MS) {
        calibratePosture();
        buttonHandled = true;
      }
    }
  }

  lastButtonReading = reading;
}

void updateSensors() {
  int16_t axr, ayr, azr;
  if (mpu_read_accel_raw(axr, ayr, azr)) {
    float pitch = computePitchDeg((float)axr, (float)ayr, (float)azr);
    app.sensors.pitchDeg = pitch;

    float delta = pitch - app.baselinePitchDeg;
    if (delta < 0.0f) delta = -delta;
    app.sensors.postureDeltaDeg = delta;
  }

  float d = readDistanceCm();
  if (d > 0.0f) {
    app.sensors.distanceCm = d;
    app.sensors.distanceValid = true;
  } else {
    app.sensors.distanceValid = false;
  }

  app.sensors.ldrRaw = adc_read_channel(0);
}

void updateAlerts() {
  uint32_t now = millis();

  bool postureNow = app.sensors.postureDeltaDeg >= POSTURE_WARN_DEG;
  bool distanceNow = app.sensors.distanceValid && app.sensors.distanceCm <= DISTANCE_WARN_CM;

  app.alerts.postureCritical  = app.sensors.postureDeltaDeg >= POSTURE_CRITICAL_DEG;
  app.alerts.distanceCritical = app.sensors.distanceValid && (app.sensors.distanceCm <= DISTANCE_CRITICAL_CM);

  if (postureNow) {
    if (postureStartMs == 0) postureStartMs = now;
    if (now - postureStartMs >= POSTURE_HOLD_MS) app.alerts.posture = true;
  } else {
    postureStartMs = 0;
    app.alerts.posture = false;
    app.alerts.postureCritical = false;
  }

  if (distanceNow) {
    if (distanceStartMs == 0) distanceStartMs = now;
    if (now - distanceStartMs >= DISTANCE_HOLD_MS) app.alerts.distance = true;
  } else {
    distanceStartMs = 0;
    app.alerts.distance = false;
    app.alerts.distanceCritical = false;
  }
}

void updateBuzzer() {
  uint32_t now = millis();

  if (!app.alerts.any()) {
    noTone(9);
    buzzerActive = false;
    return;
  }

  if (app.alerts.both() || app.alerts.postureCritical || app.alerts.distanceCritical) {
    if (!buzzerActive && now - lastBuzzerTick >= 250) {
      tone(9, 1600);
      buzzerActive = true;
      lastBuzzerTick = now;
    } else if (buzzerActive && now - lastBuzzerTick >= 120) {
      noTone(9);
      buzzerActive = false;
      lastBuzzerTick = now;
    }
    return;
  }

  if (app.alerts.distance) {
    if (!buzzerActive && now - lastBuzzerTick >= 700) {
      tone(9, 1100);
      buzzerActive = true;
      lastBuzzerTick = now;
    } else if (buzzerActive && now - lastBuzzerTick >= 90) {
      noTone(9);
      buzzerActive = false;
      lastBuzzerTick = now;
    }
    return;
  }

  if (app.alerts.posture) {
    if (!buzzerActive && now - lastBuzzerTick >= 900) {
      tone(9, 900);
      buzzerActive = true;
      lastBuzzerTick = now;
    } else if (buzzerActive && now - lastBuzzerTick >= 90) {
      noTone(9);
      buzzerActive = false;
      lastBuzzerTick = now;
    }
  }
}

void updateLeds() {
  int newSeverity = computeSeverity();
  uint32_t now = millis();

  if (newSeverity != currentSeverity) {
    if (newSeverity != candidateSeverity) {
      candidateSeverity = newSeverity;
      candidateSince = now;
    } else if (now - candidateSince >= SEVERITY_STABLE_MS) {
      currentSeverity = candidateSeverity;
    }
  } else {
    candidateSeverity = currentSeverity;
    candidateSince = now;
  }

  uint8_t brightness = computeLedBrightness(app.sensors.ldrRaw);
  leds_apply_severity(currentSeverity, brightness);
}

void updateLCD() {
  if (millis() - lastLcdUpdate < LCD_PERIOD_MS) return;
  lastLcdUpdate = millis();

  char line0[17];
  char line1[17];

  if (app.calibrating) {
    lcd_print_line(0, "Calibrare...");
    lcd_print_line(1, "Stai drept");
    return;
  }

  if (app.alerts.both()) {
    lcd_print_line(0, "Te cocosezi!");
    lcd_print_line(1, "Prea aproape!");
    return;
  }

  if (app.alerts.posture) {
    char num[8];
    dtostrf(app.sensors.postureDeltaDeg, 0, 1, num);
    snprintf(line0, sizeof(line0), "Te cocosezi!");
    snprintf(line1, sizeof(line1), "Unghi:%s", num);
    lcd_print_line(0, line0);
    lcd_print_line(1, line1);
    return;
  }

  if (app.alerts.distance) {
    if (app.sensors.distanceValid) {
      char num[8];
      dtostrf(app.sensors.distanceCm, 0, 1, num);
      snprintf(line0, sizeof(line0), "Prea aproape!");
      snprintf(line1, sizeof(line1), "Dist:%s cm", num);
    } else {
      snprintf(line0, sizeof(line0), "Prea aproape!");
      snprintf(line1, sizeof(line1), "Dist:---");
    }
    lcd_print_line(0, line0);
    lcd_print_line(1, line1);
    return;
  }

  if (app.alertModeOnly) {
    lcd_print_line(0, "ErgoDesk");
    lcd_print_line(1, "Alert Mode");
  } else {
    char tBuf[8];
    char dBuf[8];

    dtostrf(app.sensors.postureDeltaDeg, 0, 1, tBuf);
    if (app.sensors.distanceValid) dtostrf(app.sensors.distanceCm, 0, 1, dBuf);
    else strcpy(dBuf, "---");

    snprintf(line0, sizeof(line0), "T:%s D:%s", tBuf, dBuf);
    snprintf(line1, sizeof(line1), "Lum:%u", app.sensors.ldrRaw);

    lcd_print_line(0, line0);
    lcd_print_line(1, line1);
  }
}