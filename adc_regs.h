#pragma once
#include <Arduino.h>

void adc_init();
uint16_t adc_read_channel(uint8_t ch);