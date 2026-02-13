#pragma once

#include <Arduino.h>

uint8_t spi_test_read_reg(uint16_t reg12);
void spi_test_write_reg(uint16_t reg12, uint8_t data);
void spi_test_write_readback(uint16_t reg12, uint8_t value);
