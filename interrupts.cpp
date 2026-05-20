#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "interrupts.h"

volatile bool g_buttonEdgeFlag = false;

ISR(INT0_vect) {
  g_buttonEdgeFlag = true;
}

void interrupts_init() {
  EICRA |= (1 << ISC00);
  EICRA &= ~(1 << ISC01);
  EIMSK |= (1 << INT0);
}