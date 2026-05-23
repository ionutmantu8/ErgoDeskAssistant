#include <Arduino.h>
#include <avr/io.h>
#include "adc_regs.h"

void adc_init() {
  ADMUX = (1 << REFS0);//Set ADC multiplexer to AVcc reference and ADC0 as input
  ADCSRA = (1 << ADEN) |
           (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set in ADC status control register A:
                                                      // Enable ADC and set prescaler to 128 (16MHz/128 = 125KHz)
} 

uint16_t adc_read_channel(uint8_t ch) {
  ADMUX = (ADMUX & 0xF0) | (ch & 0x0F); // Keep superior part of the register and take the last 4 bits of the channel number
  ADCSRA |= (1 << ADSC); // Start conversion
  while (ADCSRA & (1 << ADSC));// Wait for conversion to complete
  return ADC;
}