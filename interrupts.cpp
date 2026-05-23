#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "interrupts.h"

volatile bool g_buttonEdgeFlag = false; // volatile because it's modified in an ISR

ISR(INT0_vect) {
  g_buttonEdgeFlag = true; // Interupt routine to set the flag when the button is pressed (rising edge)
}

void interrupts_init() {
  EICRA |= (1 << ISC00);
  EICRA &= ~(1 << ISC01); // Set ISC00 and ISC01 to 10 for falling edge trigger
  EIMSK |= (1 << INT0); // Activate INT0 interupt
}