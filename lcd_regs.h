#pragma once
#include <Arduino.h>

extern uint8_t lcd_backlight;

void lcd_init_regs();
void lcd_clear();
void lcd_setCursor(uint8_t col, uint8_t row);
void lcd_print(const char* s);
void lcd_print_line(uint8_t row, const char* text);

void lcd_show_boot();
void lcd_show_calibration(uint8_t step, uint8_t total);