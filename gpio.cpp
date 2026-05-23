#include <Arduino.h>
#include <avr/io.h>
#include "gpio.h"

void gpio_init() {
  DDRD &= ~(1 << PD2); // Set button as input
  PORTD |= (1 << PD2); // Enable pull-up resistor for button

  DDRD |= (1 << PD7); // Set trigger as output
  PORTD &= ~(1 << PD7); // Initialize trigger to low

  DDRB &= ~(1 << PB0); // Set echo as input
  DDRB |= (1 << PB1); // Set buzzer as output
}

bool button_raw_read() {
  return (PIND & (1 << PD2)) != 0; // read button state (active low)
}

void trig_low() {
  PORTD &= ~(1 << PD7); // Set trigger pin low
}

void trig_high() {
  PORTD |= (1 << PD7); // Set trigger pin high
}

bool echo_read() {
  return (PINB & (1 << PB0)) != 0; // Read echo pin state
}