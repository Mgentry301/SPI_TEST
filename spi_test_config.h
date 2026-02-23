#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <stddef.h>

// ── Preset selector ──────────────────────────────────────────
// Change this line to switch presets:
//   PRESET_ADDR3       — hardcoded CHIP_ADDR=3 (known-good board)
//   PRESET_ADDR_SCAN   — scans all 4 addresses at startup
//#define SPI_TEST_PRESET  PRESET_ADDR3

#define PRESET_ADDR3      0
#define PRESET_ADDR_SCAN  1

// ── Register map ─────────────────────────────────────────────
#define SPI_TEST_REG_SOFTCTL      0x000
#define SPI_TEST_REG_SCRATCHPAD_A 0x00A

// ── Runtime config (set by apply_spi_preset) ─────────────────
extern uint32_t g_spi_hz;
extern uint8_t g_spi_mode;
extern uint8_t g_cs_pin;
extern uint16_t g_cs_post_us;
extern uint8_t g_chip_addr;
extern uint8_t g_sdo_active_mask;
extern uint8_t g_addrinc_mask;
extern const uint16_t* g_test_regs;
extern size_t g_test_regs_count;
extern const uint16_t* g_poll_regs;
extern size_t g_poll_regs_count;

void apply_spi_preset();
uint8_t spi_mode_to_int(uint8_t mode);
