#include <Arduino.h>
#include <avr/io.h>
#include <util/twi.h>
#include "twi_regs.h"

void twi_init() {
  TWSR = 0x00; // Prescaler = 1
  TWBR = 72;  // Speed for I2C magistral to reach aproximately 100KHz
  TWCR = (1 << TWEN);
}

bool twi_start(uint8_t address_rw) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  // Set TWINT to clear it, TWSTA to send START condition, and TWEN to enable TWI
  while (!(TWCR & (1 << TWINT))); // Wait for TWINT to be set, indicating the START condition has been transmitted

  uint8_t status = TWSR & 0xF8; // Keep only the status bits
  if (status != TW_START && status != TW_REP_START) return false; // If the status is not START or REPEATED START, return false

  TWDR = address_rw; // Load the address and R/W bit into TWDR
  TWCR = (1 << TWINT) | (1 << TWEN);// Clear TWINT to start transmission of address
  while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete

  status = TWSR & 0xF8; // Check if the address was acknowledged
  return (status == TW_MT_SLA_ACK || status == TW_MR_SLA_ACK); // Check if the device acknowledged the address (either in master transmit or master receive mode)
}

bool twi_write(uint8_t data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN); // Start transmision
  while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete

  uint8_t status = TWSR & 0xF8; // Keep only the status bits
  return (status == TW_MT_DATA_ACK); // Check if the slave acknowledged the data byte 
}

uint8_t twi_read_ack() {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Clear TWINT to start reception, enable TWI, and set TWEA to send ACK after reception
  while (!(TWCR & (1 << TWINT))); // Wait for reception to complete
  return TWDR; // Return the received data byte
}

uint8_t twi_read_nack() {
  TWCR = (1 << TWINT) | (1 << TWEN); // Clear TWINT to start reception and enable TWI, but do not set TWEA to send NACK after reception
  while (!(TWCR & (1 << TWINT))); // Wait for reception to complete
  return TWDR; // Return the received data byte
}

void twi_stop() {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO); // Clear TWINT to start transmission of STOP condition, enable TWI, and set TWSTO to send STOP condition
  while (TWCR & (1 << TWSTO));// Wait for the STOP condition to be transmitted (TWSTO will be cleared by hardware when the STOP condition is sent)
}

bool i2c_write_reg(uint8_t dev, uint8_t reg, uint8_t value) {
  if (!twi_start((dev << 1) | 0)) return false; // Addres the device with the write bit (0)
  if (!twi_write(reg)) { twi_stop(); return false; } // Write the register address, if it fails, send STOP and return false
  if (!twi_write(value)) { twi_stop(); return false; } // Write the value to the register, if it fails, send STOP and return false
  twi_stop();
  return true;
}

bool i2c_read_bytes(uint8_t dev, uint8_t startReg, uint8_t* buf, uint8_t len) {
  if (!twi_start((dev << 1) | 0)) return false; // Address the device with the write bit (0) to set the register address
  if (!twi_write(startReg)) { twi_stop(); return false; } // Write the starting register address, if it fails, send STOP and return false
  if (!twi_start((dev << 1) | 1)) return false; // Send a repeated START condition and address the device with the read bit (1) to start reading

  for (uint8_t i = 0; i < len; i++) {
    if (i < len - 1) buf[i] = twi_read_ack(); // Read a byte and send ACK
    else buf[i] = twi_read_nack(); // Read the last byte and send NACK to indicate the end of reading
  }

  twi_stop();
  return true;
}