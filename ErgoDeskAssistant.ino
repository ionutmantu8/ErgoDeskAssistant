#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"
#include "gpio.h"
#include "pwm.h"
#include "adc_regs.h"
#include "twi_regs.h"
#include "lcd_regs.h"
#include "mpu_regs.h"
#include "interrupts.h"
#include "logic.h"

void setup() {
  cli();

  gpio_init();
  adc_init();
  twi_init();
  pwm_init_leds();
  interrupts_init();

  sei();

  lcd_init_regs();
  lcd_show_boot();

  if (!mpu_init_regs()) {
    lcd_clear();
    lcd_print_line(0, "MPU fail");
    while (1);
  }

  calibratePosture();
}

void loop() {
  if (g_buttonEdgeFlag || !button_raw_read()) {
    g_buttonEdgeFlag = false;
    updateButton();
  } else {
    updateButton();
  }

  if (millis() - lastSensorRead >= SENSOR_PERIOD_MS) {
    lastSensorRead = millis();
    updateSensors();
    updateAlerts();
  }

  updateLeds();
  updateBuzzer();
  updateLCD();
}