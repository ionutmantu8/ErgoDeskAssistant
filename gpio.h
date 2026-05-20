#pragma once
#include <Arduino.h>

void gpio_init();

bool button_raw_read();
void trig_low();
void trig_high();
bool echo_read();