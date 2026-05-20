#pragma once
#include <Arduino.h>

void twi_init();
bool twi_start(uint8_t address_rw);
bool twi_write(uint8_t data);
uint8_t twi_read_ack();
uint8_t twi_read_nack();
void twi_stop();

bool i2c_write_reg(uint8_t dev, uint8_t reg, uint8_t value);
bool i2c_read_bytes(uint8_t dev, uint8_t startReg, uint8_t* buf, uint8_t len);