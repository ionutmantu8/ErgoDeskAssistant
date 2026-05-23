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
  cli(); // Disable global interrupts during low-level hardware initialization

  gpio_init();        // Configure GPIO pins for button, ultrasonic sensor and buzzer
  adc_init();         // Initialize the ADC used to read the photoresistor
  twi_init();         // Initialize the TWI/I2C peripheral for LCD and MPU6050
  pwm_init_leds();    // Initialize hardware PWM for the LEDs
  interrupts_init();  // Configure the external interrupt used for the button

  sei(); // Re-enable global interrupts after initialization is complete

  lcd_init_regs();    // Initialize the LCD in 4-bit mode through the I2C expander
  lcd_show_boot();    // Show the startup message

  if (!mpu_init_regs()) {
    lcd_clear();
    lcd_print_line(0, "MPU fail"); // Show an error message if MPU6050 init fails
    while (1);                     // Stop here because the sensor is required by the application
  }

  calibratePosture(); // Take initial samples and compute the reference posture baseline
}

void loop() {
  // Update button handling; the interrupt only sets a flag, the real logic is done here
  if (g_buttonEdgeFlag || !button_raw_read()) {
    g_buttonEdgeFlag = false;
    updateButton();
  } else {
    updateButton();
  }

  // Read sensors and refresh alert states periodically
  if (millis() - lastSensorRead >= SENSOR_PERIOD_MS) {
    lastSensorRead = millis();
    updateSensors();
    updateAlerts();
  }

  updateLeds();    // Update LED color and brightness based on current severity and LDR value
  updateBuzzer();  // Update buzzer pattern depending on the active alerts
  updateLCD();     // Refresh the LCD with either alert messages or measured values
}