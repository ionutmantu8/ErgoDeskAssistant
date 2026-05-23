#include <Arduino.h>
#include "config.h"
#include "twi_regs.h"
#include "lcd_regs.h"

uint8_t lcd_backlight = LCD_BL;
// Stores the current backlight state so every I2C write keeps the backlight on

static void lcd_expander_write(uint8_t data) {
  twi_start((LCD_ADDR << 1) | 0);
  // Start an I2C write transaction to the LCD backpack address

  twi_write(data | lcd_backlight);
  // Send one byte to the PCF8574 expander, while preserving the backlight bit

  twi_stop();
  // End the I2C transaction
}

static void lcd_pulse_enable(uint8_t data) {
  lcd_expander_write(data | LCD_EN);
  // Set the EN bit high so the LCD latches the current nibble

  delayMicroseconds(1);
  // Short enable pulse width

  lcd_expander_write(data & ~LCD_EN);
  // Clear the EN bit to complete the latch pulse

  delayMicroseconds(50);
  // Wait for the LCD to process the nibble
}

static void lcd_write4bits(uint8_t nibble) {
  lcd_expander_write(nibble);
  // Put the nibble on the LCD data lines through the I2C expander

  lcd_pulse_enable(nibble);
  // Pulse the enable line so the LCD reads that nibble
}

static void lcd_send(uint8_t value, uint8_t mode) {
  uint8_t high = (value & 0xF0) | mode;
  // Take the high nibble of the byte and add the RS mode bit

  uint8_t low  = ((value << 4) & 0xF0) | mode;
  // Shift the low nibble into the upper 4 bits and add the RS mode bit

  lcd_write4bits(high);
  // Send the high nibble first

  lcd_write4bits(low);
  // Then send the low nibble
}

static void lcd_command(uint8_t cmd) {
  lcd_send(cmd, 0);
  // Send a command byte with RS = 0
}

static void lcd_data(uint8_t data) {
  lcd_send(data, LCD_RS);
  // Send a data byte with RS = 1
}

void lcd_init_regs() {
  delay(50);
  // Wait for LCD power-up

  lcd_write4bits(0x30);
  // Initialization sequence: first wake-up nibble

  delay(5);

  lcd_write4bits(0x30);
  // Second wake-up nibble

  delayMicroseconds(150);

  lcd_write4bits(0x30);
  // Third wake-up nibble

  lcd_write4bits(0x20);
  // Switch LCD to 4-bit mode

  lcd_command(0x28);
  // Function set: 4-bit mode, 2 lines, 5x8 font

  lcd_command(0x0C);
  // Display ON, cursor OFF, blink OFF

  lcd_command(0x06);
  // Entry mode: move cursor right after each character

  lcd_command(0x01);
  // Clear display

  delay(2);
  // Clear command needs extra time
}

void lcd_clear() {
  lcd_command(0x01);
  // Clear the LCD screen

  delay(2);
  // Wait for the clear command to finish
}

void lcd_setCursor(uint8_t col, uint8_t row) {
  static const uint8_t row_offsets[] = {0x00, 0x40};
  // DDRAM start addresses for row 0 and row 1 on a 16x2 LCD

  lcd_command(0x80 | (col + row_offsets[row]));
  // Set DDRAM address to move the cursor to the requested column and row
}

void lcd_print(const char* s) {
  while (*s) lcd_data(*s++);
  // Send characters one by one until the end of the string
}

void lcd_print_line(uint8_t row, const char* text) {
  char buf[17];
  uint8_t i = 0;

  while (i < 16 && text[i]) {
    buf[i] = text[i];
    i++;
  }
  // Copy up to 16 visible characters from the input string

  while (i < 16) buf[i++] = ' ';
  // Fill the rest of the line with spaces so old characters are overwritten

  buf[16] = '\0';

  lcd_setCursor(0, row);
  // Move cursor to the beginning of the requested row

  lcd_print(buf);
  // Print exactly one full LCD row
}

void lcd_show_boot() {
  lcd_clear();
  // Clear display before showing boot message

  lcd_print_line(0, "ErgoDesk Assist");
  // First line: project name

  lcd_print_line(1, "Boot...");
  // Second line: startup message
}

void lcd_show_calibration(uint8_t step, uint8_t total) {
  char line[17];

  snprintf(line, sizeof(line), "%u/%u", step, total);
  // Build a progress string like "3/25"

  lcd_clear();
  // Clear display before showing calibration progress

  lcd_print_line(0, "Calibrare...");
  // First line: calibration status

  lcd_print_line(1, line);
  // Second line: current sample / total samples
}