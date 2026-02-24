#include "spi_test_spi.h"

#include "spi_test_config.h"

#include <SPI.h>
#include <Linduino.h>
#include <LT_SPI.h>

// Print a uint8_t as zero-padded 2-digit hex (e.g. "0A")
void print_hex8(uint8_t val) {
  if (val < 0x10) Serial.print('0');
  Serial.print(val, HEX);
}

// ADMV1455 SPI frame: 24 bits, clocked MSB-first
//   Byte 0 (first on wire): header[15:8]  — R/W, BR, CHIP_ADDR[1:0], REG[11:8]
//   Byte 1:                  header[7:0]   — REG[7:0]
//   Byte 2 (last on wire):   data byte
//
// NOTE: LT_SPI's spi_transfer_block() sends bytes in REVERSE index order
//       (LSByte-first), which is wrong for the ADMV1455.  We drive CS
//       manually and clock bytes out in forward order instead.

uint8_t spi_test_read_reg(uint16_t reg) {
  uint16_t header = (1u << 15) | ((g_chip_addr & 0x03u) << 12) | (reg & 0x0FFFu);
  SPI.beginTransaction(SPISettings(g_spi_hz, MSBFIRST, g_spi_mode));
  output_low(g_cs_pin);
  SPI.transfer((uint8_t)(header >> 8));   // header MSB
  SPI.transfer((uint8_t)(header));        // header LSB
  uint8_t val = SPI.transfer(0x00);       // read data byte
  output_high(g_cs_pin);
  SPI.endTransaction();
  return val;
}

void spi_test_write_reg(uint16_t reg, uint8_t data) {
  uint16_t header = ((g_chip_addr & 0x03u) << 12) | (reg & 0x0FFFu);
  SPI.beginTransaction(SPISettings(g_spi_hz, MSBFIRST, g_spi_mode));
  output_low(g_cs_pin);
  SPI.transfer((uint8_t)(header >> 8));   // header MSB
  SPI.transfer((uint8_t)(header));        // header LSB
  SPI.transfer(data);                     // write data byte
  output_high(g_cs_pin);
  SPI.endTransaction();
}

void spi_test_write_readback(uint16_t reg, uint8_t value) {
  spi_test_write_reg(reg, value);
  uint8_t rb = spi_test_read_reg(reg);
  Serial.print(F("  reg 0x"));
  if (reg < 0x10) Serial.print('0');
  Serial.print(reg, HEX);
  Serial.print(F(" W:0x")); print_hex8(value);
  Serial.print(F(" R:0x")); print_hex8(rb);
  if (rb != value) {
    Serial.print(F(" [MISMATCH]"));
  }
  Serial.println();
}
