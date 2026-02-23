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

// MOSI→MISO loopback self-test.
// Temporarily disconnects CS so the device ignores traffic,
// then clocks out test bytes and checks that MISO returns them.
// Requires a physical jumper from MOSI (pin 11) to MISO (pin 12).
bool spi_loopback_test() {
  const uint8_t patterns[] = {0x55, 0xAA, 0xFF, 0x00, 0xA5};
  bool pass = true;
  SPI.beginTransaction(SPISettings(g_spi_hz, MSBFIRST, g_spi_mode));
  output_high(g_cs_pin);                       // keep CS high — no device interaction
  for (uint8_t i = 0; i < sizeof(patterns); i++) {
    uint8_t rx = SPI.transfer(patterns[i]);
    Serial.print(F("  TX=0x")); print_hex8(patterns[i]);
    Serial.print(F(" RX=0x")); print_hex8(rx);
    if (rx == patterns[i]) {
      Serial.println(F(" [OK]"));
    } else {
      Serial.println(F(" [FAIL]"));
      pass = false;
    }
  }
  SPI.endTransaction();
  return pass;
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

// Scan CHIP_ADDR 0-3: write SDO_ACTIVE, then readback.
// Returns first address with valid response, or -1.
int8_t spi_scan_chip_addr() {
  uint8_t saved_addr = g_chip_addr;
  int8_t found = -1;

  Serial.println(F("\nAddr scan (0-3):"));
  for (uint8_t addr = 0; addr < 4; addr++) {
    g_chip_addr = addr;
    spi_test_write_reg(SPI_TEST_REG_SOFTCTL, g_sdo_active_mask);
    delay(5);
    uint8_t rb = spi_test_read_reg(SPI_TEST_REG_SOFTCTL);
    Serial.print(F("  ADDR ")); Serial.print(addr);
    Serial.print(F(" -> 0x")); print_hex8(rb);
    if (rb == g_sdo_active_mask) {
      Serial.println(F(" [MATCH]"));
      if (found < 0) found = (int8_t)addr;
    } else if (rb == 0xFF) {
      Serial.println(F(" [0xFF]"));
    } else if (rb == 0x00) {
      Serial.println(F(" [0x00]"));
    } else {
      Serial.print(F(" [?0x")); print_hex8(rb); Serial.println(F("]"));
      if (found < 0) found = (int8_t)addr;
    }
  }

  if (found >= 0) {
    g_chip_addr = (uint8_t)found;
    Serial.print(F("Detected addr: ")); Serial.println(found);
  } else {
    g_chip_addr = saved_addr;
    Serial.println(F("No chip responded."));
  }
  return found;
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
