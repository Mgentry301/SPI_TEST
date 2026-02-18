#include "spi_test_config.h"

uint32_t g_spi_hz = 1000000UL;
uint8_t g_spi_mode = SPI_MODE0;
uint8_t g_cs_pin = 10;
uint16_t g_cs_post_us = 0;
uint8_t g_chip_addr = 0;
uint8_t g_sdo_active_mask = 0x08;
uint8_t g_addrinc_mask = 0x00;
const uint16_t* g_test_regs = nullptr;
size_t g_test_regs_count = 0;
const uint16_t* g_poll_regs = nullptr;
size_t g_poll_regs_count = 0;
void apply_spi_preset() {
  // ADMV1455 minimal defaults for register 0x00A access.
  g_spi_hz = 1000000UL;
  g_spi_mode = SPI_MODE0;
  g_cs_pin = 10;
  g_cs_post_us = 0;
  g_chip_addr = 0;
  g_addrinc_mask = 0x00;
  g_sdo_active_mask = 0x08; // ADMV1455: reg 0x000 bit 3

  static const uint16_t admv1455_regs[] = {0x00A};
  static const uint16_t admv1455_poll[] = {0x00A};
  g_test_regs = admv1455_regs;
  g_test_regs_count = sizeof(admv1455_regs) / sizeof(admv1455_regs[0]);
  g_poll_regs = admv1455_poll;
  g_poll_regs_count = sizeof(admv1455_poll) / sizeof(admv1455_poll[0]);
}

uint8_t spi_mode_to_int(uint8_t mode) {
  switch (mode) {
    case SPI_MODE0: return 0;
    case SPI_MODE1: return 1;
    case SPI_MODE2: return 2;
    case SPI_MODE3: return 3;
    default: return 0xFF;
  }
}
