#include "spi_test_spi.h"

#include <SPI.h>

#include "spi_test_config.h"
#include "spi_test_utils.h"

static inline uint16_t mask_reg12(uint16_t reg) {
  return (uint16_t)(reg & 0x0FFFu);
}

static inline uint8_t mask_chip_addr2(uint8_t chip_addr) {
  return (uint8_t)(chip_addr & 0x03u);
}

// Datasheet header: [15]=R/W, [14]=BR, [13:12]=CHIP_ADDR, [11:0]=REG_ADDR
static inline uint16_t make_header(bool is_read, bool br, uint8_t chip_addr, uint16_t reg12) {
  const uint16_t rw_bit = (uint16_t)(is_read ? 1u : 0u) << 15;
  const uint16_t br_bit = (uint16_t)(br ? 1u : 0u) << 14;
  const uint16_t ca_bits = (uint16_t)(mask_chip_addr2(chip_addr)) << 12;
  const uint16_t ra_bits = mask_reg12(reg12);
  return (uint16_t)(rw_bit | br_bit | ca_bits | ra_bits);
}

static inline void cs_low() {
  digitalWrite(g_cs_pin, LOW);
}

static inline void cs_high() {
  digitalWrite(g_cs_pin, HIGH);
  if (g_cs_post_us > 0) {
    delayMicroseconds(g_cs_post_us);
  }
}

static inline void spi_begin_txn() {
  SPI.beginTransaction(SPISettings(g_spi_hz, MSBFIRST, g_spi_mode));
}

static inline void spi_end_txn() {
  SPI.endTransaction();
}

uint8_t spi_test_read_reg(uint16_t reg12) {
  spi_begin_txn();
  cs_low();

  // ADMV1455 16-bit header: [15]=R/W, [14]=BR, [13:12]=CHIP_ADDR, [11:0]=REG
  const uint16_t header = make_header(true /*read*/, false /*br*/, g_chip_addr, reg12);
  SPI.transfer((uint8_t)((header >> 8) & 0xFFu));
  SPI.transfer((uint8_t)(header & 0xFFu));

  uint8_t val = SPI.transfer(0x00);
  cs_high();
  spi_end_txn();

  return val;
}

void spi_test_write_reg(uint16_t reg12, uint8_t data) {
  spi_begin_txn();
  cs_low();

  const uint16_t header = make_header(false /*write*/, false /*br*/, g_chip_addr, reg12);
  SPI.transfer((uint8_t)((header >> 8) & 0xFFu));
  SPI.transfer((uint8_t)(header & 0xFFu));

  SPI.transfer(data);
  cs_high();
  spi_end_txn();
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
