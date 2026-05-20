#include <Arduino.h>
#include <avr/io.h>
#include "adc_regs.h"

void adc_init() {
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) |
           (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read_channel(uint8_t ch) {
  ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));
  return ADC;
}