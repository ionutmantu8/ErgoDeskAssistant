#include <Arduino.h>
#include "config.h"
#include "twi_regs.h"
#include "lcd_regs.h"

uint8_t lcd_backlight = LCD_BL;

static void lcd_expander_write(uint8_t data) {
  twi_start((LCD_ADDR << 1) | 0);
  twi_write(data | lcd_backlight);
  twi_stop();
}

static void lcd_pulse_enable(uint8_t data) {
  lcd_expander_write(data | LCD_EN);
  delayMicroseconds(1);
  lcd_expander_write(data & ~LCD_EN);
  delayMicroseconds(50);
}

static void lcd_write4bits(uint8_t nibble) {
  lcd_expander_write(nibble);
  lcd_pulse_enable(nibble);
}

static void lcd_send(uint8_t value, uint8_t mode) {
  uint8_t high = (value & 0xF0) | mode;
  uint8_t low  = ((value << 4) & 0xF0) | mode;
  lcd_write4bits(high);
  lcd_write4bits(low);
}

static void lcd_command(uint8_t cmd) {
  lcd_send(cmd, 0);
}

static void lcd_data(uint8_t data) {
  lcd_send(data, LCD_RS);
}

void lcd_init_regs() {
  delay(50);
  lcd_write4bits(0x30);
  delay(5);
  lcd_write4bits(0x30);
  delayMicroseconds(150);
  lcd_write4bits(0x30);
  lcd_write4bits(0x20);
  lcd_command(0x28);
  lcd_command(0x0C);
  lcd_command(0x06);
  lcd_command(0x01);
  delay(2);
}

void lcd_clear() {
  lcd_command(0x01);
  delay(2);
}

void lcd_setCursor(uint8_t col, uint8_t row) {
  static const uint8_t row_offsets[] = {0x00, 0x40};
  lcd_command(0x80 | (col + row_offsets[row]));
}

void lcd_print(const char* s) {
  while (*s) lcd_data(*s++);
}

void lcd_print_line(uint8_t row, const char* text) {
  char buf[17];
  uint8_t i = 0;
  while (i < 16 && text[i]) {
    buf[i] = text[i];
    i++;
  }
  while (i < 16) buf[i++] = ' ';
  buf[16] = '\0';
  lcd_setCursor(0, row);
  lcd_print(buf);
}

void lcd_show_boot() {
  lcd_clear();
  lcd_print_line(0, "ErgoDesk Assist");
  lcd_print_line(1, "Boot...");
}

void lcd_show_calibration(uint8_t step, uint8_t total) {
  char line[17];
  snprintf(line, sizeof(line), "%u/%u", step, total);
  lcd_clear();
  lcd_print_line(0, "Calibrare...");
  lcd_print_line(1, line);
}