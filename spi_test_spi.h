#pragma once

#include <Arduino.h>
#include <LT_SPI.h>

void print_hex8(uint8_t val);
bool spi_loopback_test();

uint8_t spi_test_read_reg(uint16_t reg);
void spi_test_write_reg(uint16_t reg, uint8_t data);
void spi_test_write_readback(uint16_t reg, uint8_t value);
