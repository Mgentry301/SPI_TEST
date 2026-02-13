#include "spi_test_spi.h"

#include <SPI.h>

#include "spi_test_config.h"
#include "spi_test_utils.h"

uint8_t spi_test_read_reg(uint16_t reg12) {
  digitalWrite(g_cs_pin, LOW);

  if (g_spi_protocol == 1) {
    // AD9082-style address phase with read flag in the first address byte.
    if (g_spi_addr_bytes == 2) {
      uint8_t msb = (uint8_t)((reg12 >> 8) & 0xFFu);
      uint8_t lsb = (uint8_t)(reg12 & 0xFFu);
      msb = (uint8_t)(msb | g_spi_read_flag);
      SPI.transfer(msb);
      SPI.transfer(lsb);
    } else {
      uint8_t b2 = (uint8_t)((reg12 >> 16) & 0xFFu);
      uint8_t b1 = (uint8_t)((reg12 >> 8) & 0xFFu);
      uint8_t b0 = (uint8_t)(reg12 & 0xFFu);
      b2 = (uint8_t)(b2 | g_spi_read_flag);
      SPI.transfer(b2);
      SPI.transfer(b1);
      SPI.transfer(b0);
    }
    for (uint8_t i = 0; i < g_spi_read_dummy_bytes; ++i) {
      SPI.transfer(0x00);
    }
  } else {
    // ADMV1455/ADMV1355-style 16-bit header: [15]=R/W, [14]=BR, [13:12]=CHIP_ADDR, [11:0]=REG
    uint16_t header = 0;
    header |= (1u << 15);
    header |= ((g_chip_addr & 0x3) << 12);
    header |= (reg12 & 0x0FFF);
    SPI.transfer((uint8_t)(header >> 8));
    SPI.transfer((uint8_t)(header & 0xFF));
  }

  uint8_t val = SPI.transfer(0x00);
  digitalWrite(g_cs_pin, HIGH);

  return val;
}

void spi_test_write_reg(uint16_t reg12, uint8_t data) {
  digitalWrite(g_cs_pin, LOW);

  if (g_spi_protocol == 1) {
    if (g_spi_addr_bytes == 2) {
      uint8_t msb = (uint8_t)((reg12 >> 8) & 0xFFu);
      uint8_t lsb = (uint8_t)(reg12 & 0xFFu);
      msb = (uint8_t)(msb | g_spi_write_flag);
      SPI.transfer(msb);
      SPI.transfer(lsb);
    } else {
      uint8_t b2 = (uint8_t)((reg12 >> 16) & 0xFFu);
      uint8_t b1 = (uint8_t)((reg12 >> 8) & 0xFFu);
      uint8_t b0 = (uint8_t)(reg12 & 0xFFu);
      b2 = (uint8_t)(b2 | g_spi_write_flag);
      SPI.transfer(b2);
      SPI.transfer(b1);
      SPI.transfer(b0);
    }
  } else {
    uint16_t header = 0;
    header |= ((g_chip_addr & 0x3) << 12);
    header |= (reg12 & 0x0FFF);
    SPI.transfer((uint8_t)(header >> 8));
    SPI.transfer((uint8_t)(header & 0xFF));
  }

  SPI.transfer(data);
  digitalWrite(g_cs_pin, HIGH);
}

void spi_test_write_readback(uint16_t reg12, uint8_t value) {
  spi_test_write_reg(reg12, value);
  uint8_t rb = spi_test_read_reg(reg12);
  Serial.print("  reg 0x");
  if (reg12 < 0x10) Serial.print('0');
  Serial.print(reg12, HEX);
  Serial.print(" write 0x"); print_hex8(value);
  Serial.print(" read 0x"); print_hex8(rb);
  if (rb != value) {
    Serial.print(" [MISMATCH]");
  }
  Serial.println();
}
