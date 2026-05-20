#include <Arduino.h>
#include <avr/io.h>
#include "gpio.h"

void gpio_init() {
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRD |= (1 << PD7);
  PORTD &= ~(1 << PD7);

  DDRB &= ~(1 << PB0);
  DDRB |= (1 << PB1);
}

bool button_raw_read() {
  return (PIND & (1 << PD2)) != 0;
}

void trig_low() {
  PORTD &= ~(1 << PD7);
}

void trig_high() {
  PORTD |= (1 << PD7);
}

bool echo_read() {
  return (PINB & (1 << PB0)) != 0;
}