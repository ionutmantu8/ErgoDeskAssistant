#pragma once
#include <Arduino.h>

void pwm_init_leds();
void led_set_green(uint8_t value);
void led_set_yellow(uint8_t value);
void led_set_blue(uint8_t value);
void leds_off();