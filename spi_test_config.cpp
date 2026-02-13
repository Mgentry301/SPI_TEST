#include "spi_test_config.h"

uint32_t g_spi_hz = 1000000UL;
uint8_t g_spi_mode = SPI_MODE0;
uint8_t g_cs_pin = 10;
uint8_t g_chip_addr = 0;
uint8_t g_sdo_active_mask = 0x08;
uint8_t g_addrinc_mask = 0x00;
uint8_t g_spi_protocol = 0;
uint8_t g_spi_addr_bytes = 2;
uint8_t g_spi_read_flag = 0x80;
uint8_t g_spi_write_flag = 0x00;
uint8_t g_spi_read_dummy_bytes = 0;
const uint16_t* g_test_regs = nullptr;
size_t g_test_regs_count = 0;
bool g_use_scratchpad = true;
uint16_t g_rw_reg = 0x000A;

const char* preset_name(uint8_t preset) {
  switch (preset) {
    case SPI_TEST_PRESET_ADMV1455_MINIMAL: return "ADMV1455_MINIMAL";
    case SPI_TEST_PRESET_ADMV1355_MINIMAL: return "ADMV1355_MINIMAL";
    case SPI_TEST_PRESET_AD9082_MINIMAL: return "AD9082_MINIMAL";
    default: return "UNKNOWN";
  }
}

void apply_spi_preset(uint8_t preset) {
  // Match ADMV13x5 and AD9082 Linduino SPI configuration defaults for minimal presets.
  g_spi_hz = 1000000UL;
  g_spi_mode = SPI_MODE0;
  g_cs_pin = 10;
  g_chip_addr = 0;
  g_spi_addr_bytes = 2;
  g_spi_read_flag = 0x80;
  g_spi_write_flag = 0x00;
  g_spi_read_dummy_bytes = 0;
  g_addrinc_mask = 0x00;
  g_use_scratchpad = true;
  g_rw_reg = 0x000A;

  static const uint16_t admv1455_regs[] = {0x000, 0x005, 0x012, 0x040, 0x050};
  static const uint16_t admv1355_regs[] = {0x000, 0x001, 0x003, 0x004, 0x005};
  static const uint16_t ad9082_regs[] = {0x0000, 0x0001, 0x0002, 0x0003};

  if (preset == SPI_TEST_PRESET_ADMV1355_MINIMAL) {
    g_sdo_active_mask = 0x40; // ADMV1355: reg 0x000 bit 6
    g_test_regs = admv1355_regs;
    g_test_regs_count = sizeof(admv1355_regs) / sizeof(admv1355_regs[0]);
    g_spi_protocol = 0; // ADMV header protocol
  } else if (preset == SPI_TEST_PRESET_AD9082_MINIMAL) {
    g_sdo_active_mask = 0x08; // AD9082: SPI_INTF_CONF_A bit 3
    g_addrinc_mask = 0x04;    // AD9082: SPI_INTF_CONF_A bit 2
    g_test_regs = ad9082_regs;
    g_test_regs_count = sizeof(ad9082_regs) / sizeof(ad9082_regs[0]);
    g_spi_protocol = 1; // AD9082 address protocol
    g_use_scratchpad = false;
    g_rw_reg = 0x0000; // SPI_INTF_CONF_A
  } else {
    g_sdo_active_mask = 0x08; // ADMV1455: reg 0x000 bit 3
    g_test_regs = admv1455_regs;
    g_test_regs_count = sizeof(admv1455_regs) / sizeof(admv1455_regs[0]);
    g_spi_protocol = 0; // ADMV header protocol
  }
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
