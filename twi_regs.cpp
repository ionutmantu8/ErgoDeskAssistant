#include <Arduino.h>
#include <avr/io.h>
#include <util/twi.h>
#include "twi_regs.h"

void twi_init() {
  TWSR = 0x00;
  TWBR = 72;
  TWCR = (1 << TWEN);
}

bool twi_start(uint8_t address_rw) {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));

  uint8_t status = TWSR & 0xF8;
  if (status != TW_START && status != TW_REP_START) return false;

  TWDR = address_rw;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));

  status = TWSR & 0xF8;
  return (status == TW_MT_SLA_ACK || status == TW_MR_SLA_ACK);
}

bool twi_write(uint8_t data) {
  TWDR = data;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));

  uint8_t status = TWSR & 0xF8;
  return (status == TW_MT_DATA_ACK);
}

uint8_t twi_read_ack() {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT)));
  return TWDR;
}

uint8_t twi_read_nack() {
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)));
  return TWDR;
}

void twi_stop() {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while (TWCR & (1 << TWSTO));
}

bool i2c_write_reg(uint8_t dev, uint8_t reg, uint8_t value) {
  if (!twi_start((dev << 1) | 0)) return false;
  if (!twi_write(reg)) { twi_stop(); return false; }
  if (!twi_write(value)) { twi_stop(); return false; }
  twi_stop();
  return true;
}

bool i2c_read_bytes(uint8_t dev, uint8_t startReg, uint8_t* buf, uint8_t len) {
  if (!twi_start((dev << 1) | 0)) return false;
  if (!twi_write(startReg)) { twi_stop(); return false; }
  if (!twi_start((dev << 1) | 1)) return false;

  for (uint8_t i = 0; i < len; i++) {
    if (i < len - 1) buf[i] = twi_read_ack();
    else buf[i] = twi_read_nack();
  }

  twi_stop();
  return true;
}