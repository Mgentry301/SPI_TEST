#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <stddef.h>

// Simple SPI bring-up for ADMV1455, ADMV1355, or AD9082 in minimal mode.
//   SPI_TEST_PRESET_ADMV1455_MINIMAL - ADMV1455 minimal (default)
//   SPI_TEST_PRESET_ADMV1355_MINIMAL - ADMV1355 minimal
//   SPI_TEST_PRESET_AD9082_MINIMAL   - AD9082 minimal
#define SPI_TEST_PRESET_ADMV1455_MINIMAL 0
#define SPI_TEST_PRESET_ADMV1355_MINIMAL 1
#define SPI_TEST_PRESET_AD9082_MINIMAL   2

#ifndef SPI_TEST_INIT_PRESET
#define SPI_TEST_INIT_PRESET SPI_TEST_PRESET_ADMV1455_MINIMAL
#endif

#define SPI_TEST_REG_SOFTCTL 0x000
#define SPI_TEST_REG_SCRATCHPAD_A 0x00A

extern uint32_t g_spi_hz;
extern uint8_t g_spi_mode;
extern uint8_t g_cs_pin;
extern uint8_t g_chip_addr;
extern uint8_t g_sdo_active_mask;
extern uint8_t g_addrinc_mask;
extern uint8_t g_spi_protocol;
extern uint8_t g_spi_addr_bytes;
extern uint8_t g_spi_read_flag;
extern uint8_t g_spi_write_flag;
extern uint8_t g_spi_read_dummy_bytes;
extern const uint16_t* g_test_regs;
extern size_t g_test_regs_count;
extern bool g_use_scratchpad;
extern uint16_t g_rw_reg;

const char* preset_name(uint8_t preset);
void apply_spi_preset(uint8_t preset);
uint8_t spi_mode_to_int(uint8_t mode);
