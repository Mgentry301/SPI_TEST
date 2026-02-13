// Select the device preset here.
//   SPI_TEST_PRESET_ADMV1455_MINIMAL
//   SPI_TEST_PRESET_ADMV1355_MINIMAL
//   SPI_TEST_PRESET_AD9082_MINIMAL
#define SPI_TEST_INIT_PRESET SPI_TEST_PRESET_ADMV1455_MINIMAL

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
  Serial.println("SPI_TEST: Minimal SPI Bring-up");
  Serial.print("Preset: "); Serial.println(preset_name(SPI_TEST_INIT_PRESET));

  apply_spi_preset(SPI_TEST_INIT_PRESET);

  Serial.print("Preset applied: "); Serial.println(preset_name(SPI_TEST_INIT_PRESET));
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

  // Ensure SDO is enabled for reliable readback.
  uint8_t reg0 = spi_test_read_reg(SPI_TEST_REG_SOFTCTL);
  spi_test_write_reg(SPI_TEST_REG_SOFTCTL, (uint8_t)(reg0 | g_sdo_active_mask | g_addrinc_mask));

  Serial.println("\nReading a few registers:");
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

  if (g_use_scratchpad) {
    Serial.println("\nWrite/readback test on scratchpad register:");
    spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0x5A);
    spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0xA5);
    Serial.println("Scratchpad write/readback: VALID (if no [MISMATCH])");
  } else {
    Serial.println("\nWrite/readback test on SPI config register:");
    uint8_t rw = spi_test_read_reg(g_rw_reg);
    spi_test_write_reg(g_rw_reg, rw);
    uint8_t rb = spi_test_read_reg(g_rw_reg);
    Serial.print("  reg 0x");
    if (g_rw_reg < 0x10) Serial.print('0');
    Serial.print(g_rw_reg, HEX);
    Serial.print(" write 0x"); print_hex8(rw);
    Serial.print(" read 0x"); print_hex8(rb);
    bool rw_ok = (rb == rw);
    if (!rw_ok) {
      Serial.print(" [MISMATCH]");
    }
    Serial.println();
    Serial.print("SPI config write/readback status: ");
    Serial.println(rw_ok ? "VALID" : "INVALID");
  }

  Serial.println("\nSetup complete.");
}

void loop() {
  static uint32_t last_ms = 0;

  if (millis() - last_ms > 1000) {
    last_ms = millis();
    uint8_t r0 = spi_test_read_reg(SPI_TEST_REG_SOFTCTL);
    Serial.print("[1s] reg 0x000 = 0x");
    print_hex8(r0);
    Serial.println();
  }
}
