#include <Arduino.h>
#include <avr/io.h>
#include "pwm.h"

void pwm_init_leds() {
  DDRD |= (1 << PD5) | (1 << PD6); // Configure D5 and D6 as outputs for Timer0 PWM channels

  TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
  // COM0A1: enable PWM output on Timer0 channel A (OC0A -> D6)
  // COM0B1: enable PWM output on Timer0 channel B (OC0B -> D5)
  // WGM00 + WGM01: select Fast PWM mode for Timer0

  TCCR0B = (1 << CS01) | (1 << CS00);
  // CS01 + CS00: set Timer0 prescaler to 64

  DDRB |= (1 << PB2); // Configure D10 as output for Timer1 PWM channel B

  TCCR1A = (1 << COM1B1) | (1 << WGM10);
  // COM1B1: enable PWM output on Timer1 channel B (OC1B -> D10)
  // WGM10: part of the Fast PWM 8-bit mode selection for Timer1

  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
  // WGM12 + WGM10: select Fast PWM 8-bit mode for Timer1
  // CS11 + CS10: set Timer1 prescaler to 64

  OCR0A = 0; // Start with LED on D6 turned off
  OCR0B = 0; // Start with LED on D5 turned off
  OCR1B = 0; // Start with LED on D10 turned off
}

void led_set_green(uint8_t value)  { OCR0B = value; } // Set PWM duty cycle for the green LED
void led_set_yellow(uint8_t value) { OCR0A = value; } // Set PWM duty cycle for the yellow LED
void led_set_blue(uint8_t value)   { OCR1B = value; } // Set PWM duty cycle for the blue LED

void leds_off() {
  OCR0A = 0; // Turn off the yellow LED
  OCR0B = 0; // Turn off the green LED
  OCR1B = 0; // Turn off the blue LED
}