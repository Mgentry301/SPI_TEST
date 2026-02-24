#include <Arduino.h>
#include <SPI.h>
#include <Linduino.h>
#include <LT_SPI.h>

#include "spi_test_config.h"
#include "spi_test_spi.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }

  Serial.println();
  Serial.println(F("SPI_TEST: ADMV1455 SPI Bring-up (reg 0x00A)"));

  // Load ADMV1455 SPI preset (registers, clock, mode, etc.)
  apply_spi_preset();

  // Initialize QuikEval SPI and connect MUX for SPI mode
  quikeval_SPI_init();
  quikeval_SPI_connect();

  Serial.print(F("SPI Hz: ")); Serial.println(g_spi_hz);
  Serial.print(F("SPI Mode: ")); Serial.println(spi_mode_to_int(g_spi_mode));
  Serial.print(F("CHIP_ADDR: ")); Serial.println(g_chip_addr);
  Serial.print(F("CS pin: ")); Serial.println(g_cs_pin);
  Serial.print(F("QuikEval GPIO pin: ")); Serial.println(QUIKEVAL_GPIO);
  Serial.print(F("QuikEval MUX pin: ")); Serial.println(QUIKEVAL_MUX_MODE_PIN);

  Serial.print(F("MUX pin state after setup: "));
  Serial.println(digitalRead(QUIKEVAL_MUX_MODE_PIN) ? F("HIGH (I2C mode)") : F("LOW (SPI mode)"));

  // Hardware diagnostic for MISO line before SPI
  Serial.println(F("\nHardware diagnostic:"));
  pinMode(MISO, INPUT);
  bool miso_state = digitalRead(MISO);
  Serial.print(F("  MISO pin (before SPI): "));
  Serial.println(miso_state ? F("HIGH") : F("LOW"));
  if (!miso_state) {
    Serial.println(F("  WARNING: MISO LOW - check wiring"));
  }

  // SPI loopback self-test (jumper MOSI pin 11 → MISO pin 12 to run)
  Serial.println(F("\nSPI loopback test (needs MOSI->MISO jumper):"));
  bool loopback_ok = spi_loopback_test();
  Serial.print(F("Loopback: "));
  Serial.println(loopback_ok ? F("PASS") : F("FAIL"));

  // Enable SDO_ACTIVE for 4-wire SPI on ADMV1455.
  // By default, SDO is disabled (3-wire mode) — the chip responds on SDIO, not SDO.
  // We must write blindly since we can't verify via SDO until it's enabled.
  // 0x18 = bit 4 (SDO_ACTIVE_R) + bit 3 (SDO_ACTIVE) → enable 4-wire SPI.
  Serial.println(F("\nEnabling SDO_ACTIVE (0x18 -> reg 0x000)..."));
  spi_test_write_reg(SPI_TEST_REG_SOFTCTL, 0x18);
  delay(10);

  // Try reading back — a 0x00 result is ambiguous (could mean write failed OR SDO not yet active).
  uint8_t reg0_rb = spi_test_read_reg(SPI_TEST_REG_SOFTCTL);
  Serial.print(F("  reg 0x000 rb: 0x")); print_hex8(reg0_rb);
  if (reg0_rb == 0x18) {
    Serial.println(F(" [OK]"));
  } else if (reg0_rb == 0x00) {
    Serial.println(F(" [AMBIGUOUS]"));
  } else if (reg0_rb == 0xFF) {
    Serial.println(F(" [FAIL-floating]"));
  } else {
    Serial.println(F(" [UNEXPECTED]"));
  }

  Serial.println(F("\nReading reg 0x00A:"));
  bool all_ok = true;
  for (size_t i = 0; i < g_test_regs_count; i++) {
    uint16_t reg = g_test_regs[i];
    uint8_t val = spi_test_read_reg(reg);
    Serial.print(F("  reg 0x"));
    if (reg < 0x10) Serial.print('0');
    Serial.print(reg, HEX);
    Serial.print(F(" = 0x")); print_hex8(val);
    if (val == 0xFF) {
      Serial.print(F(" [WARN:0xFF]"));
      all_ok = false;
    } else if (val == 0x00) {
      Serial.print(F(" [WARN:0x00]"));
      all_ok = false;
    }
    Serial.println();
  }
  Serial.print(F("Readback: "));
  Serial.println(all_ok ? F("VALID") : F("INVALID"));

  Serial.println(F("\nWrite/readback reg 0x00A:"));
  spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0x5A);
  spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0xA5);
  spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0xFF);
  spi_test_write_readback(SPI_TEST_REG_SCRATCHPAD_A, 0x00);
  Serial.println(F("W/R done (check for [MISMATCH])"));

  Serial.println(F("\nSetup complete."));
}

void loop() {
  static uint32_t last_ms = 0;

  if (millis() - last_ms > 5000) {
    last_ms = millis();
    bool all_ok = true;
    Serial.print(F("[5s] regs:"));
    for (size_t i = 0; i < g_poll_regs_count; i++) {
      uint16_t reg = g_poll_regs[i];
      uint8_t val = spi_test_read_reg(reg);
      Serial.print(F(" 0x"));
      if (reg < 0x100) Serial.print('0');
      if (reg < 0x010) Serial.print('0');
      Serial.print(reg, HEX);
      Serial.print("=");
      print_hex8(val);
      if (val == 0xFF || val == 0x00) {
        all_ok = false;
      }
    }
    Serial.print(F(" | "));
    Serial.println(all_ok ? F("VALID") : F("INVALID"));
  }
}
