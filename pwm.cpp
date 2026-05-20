#include <Arduino.h>
#include <avr/io.h>
#include "pwm.h"

void pwm_init_leds() {
  DDRD |= (1 << PD5) | (1 << PD6);
  TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
  TCCR0B = (1 << CS01) | (1 << CS00);

  DDRB |= (1 << PB2);
  TCCR1A = (1 << COM1B1) | (1 << WGM10);
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);

  OCR0A = 0;
  OCR0B = 0;
  OCR1B = 0;
}

void led_set_green(uint8_t value)  { OCR0B = value; }
void led_set_yellow(uint8_t value) { OCR0A = value; }
void led_set_blue(uint8_t value)   { OCR1B = value; }

void leds_off() {
  OCR0A = 0;
  OCR0B = 0;
  OCR1B = 0;
}