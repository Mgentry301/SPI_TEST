#include <Arduino.h>
#include <SPI.h>
#include <Linduino.h>

#include "spi_test_config.h"
#include "spi_test_spi.h"
#include "spi_test_utils.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  Serial.println();
  Serial.println("SPI_TEST: ADMV1455 SPI Bring-up (reg 0x00A)");

  apply_spi_preset();
  Serial.print("SPI Hz: "); Serial.println(g_spi_hz);
  Serial.print("SPI Mode: "); Serial.println(spi_mode_to_int(g_spi_mode));
  Serial.print("CHIP_ADDR: "); Serial.println(g_chip_addr);
  Serial.print("CS pin: "); Serial.println(g_cs_pin);
  Serial.print("QuikEval GPIO pin: "); Serial.println(QUIKEVAL_GPIO);
  Serial.print("QuikEval MUX pin: "); Serial.println(QUIKEVAL_MUX_MODE_PIN);

  pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);
  digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);
  Serial.print("MUX pin state after setup: ");
  Serial.println(digitalRead(QUIKEVAL_MUX_MODE_PIN) ? "HIGH (I2C mode)" : "LOW (SPI mode)");

  pinMode(g_cs_pin, OUTPUT);
  digitalWrite(g_cs_pin, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(g_spi_hz, MSBFIRST, g_spi_mode));

  // Ensure SDO is enabled for reliable readback on ADMV1455.
  uint8_t reg0 = spi_test_read_reg(SPI_TEST_REG_SOFTCTL);
  spi_test_write_reg(SPI_TEST_REG_SOFTCTL, (uint8_t)(reg0 | g_sdo_active_mask | g_addrinc_mask));
  delay(5);

  Serial.println("\nReading register 0x00A:");
  bool all_ok = true;
  for (size_t i = 0; i < g_test_regs_count; i++) {
    uint16_t reg = g_test_regs[i];
    uint8_t val = spi_test_read_reg(reg);
    Serial.print("  reg 0x");
    if (reg < 0x10) Serial.print('0');
    Serial.print(reg, HEX);
    Serial.print(" = 0x"); print_hex8(val);
    if (val == 0xFF) {
      Serial.print(" (WARNING: all 1s - no device response?)");
      all_ok = false;
    } else if (val == 0x00) {
      Serial.print(" (WARNING: all 0s - data line issue?)");
      all_ok = false;
    }
    Serial.println();
  }
  Serial.print("Register readback status: ");
  Serial.println(all_ok ? "VALID" : "INVALID");

  Serial.println("\nWrite/readback test on register 0x00A:");
  spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0x5A);
  spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0xA5);
  Serial.println("Register 0x00A write/readback: VALID (if no [MISMATCH])");

  Serial.println("\nSetup complete.");
}

void loop() {
  static uint32_t last_ms = 0;

  if (millis() - last_ms > 5000) {
    last_ms = millis();
    bool all_ok = true;
    Serial.print("[5s] regs:");
    for (size_t i = 0; i < g_poll_regs_count; i++) {
      uint16_t reg = g_poll_regs[i];
      uint8_t val = spi_test_read_reg(reg);
      Serial.print(" 0x");
      if (reg < 0x100) Serial.print('0');
      if (reg < 0x010) Serial.print('0');
      Serial.print(reg, HEX);
      Serial.print("=");
      print_hex8(val);
      if (val == 0xFF || val == 0x00) {
        all_ok = false;
      }
    }
    Serial.print(" | ");
    Serial.println(all_ok ? "VALID" : "INVALID");
  }
}
