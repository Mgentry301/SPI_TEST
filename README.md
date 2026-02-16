# SPI_TEST

Minimal SPI connectivity test for Linduino (DC2026) to validate communication with:
- ADMV1455 (minimal)
- ADMV1355 (minimal)
- AD9082 (minimal)

This sketch is intentionally simple and only verifies SPI wiring, chip select, and basic register read/write behavior. Full device bring-up and programming live in the part-specific projects.

## Hardware

- Linduino One (DC2026)
- One of the supported DUTs on an evaluation board or custom PCB
- USB cable for programming and serial output

### QuikEval SPI Pinout

| Signal | QuikEval Pin (J1) | Linduino Pin | Arduino Pin |
|--------|-------------------|--------------|-------------|
| SCK    | 4 (SCK/SCL)       | SCK          | 13          |
| MOSI   | 7 (MOSI/SDA)      | MOSI         | 11          |
| MISO   | 5                 | MISO         | 12          |
| CS     | 6                 | SS           | 10          |
| GPIO   | 14                | GPIO         | 9           |
| GND    | 3, 8, 13          | GND          | GND         |

Note: The QuikEval MUX pin (Linduino D8) must be LOW for SPI mode. The sketch drives it LOW in setup.

## Files

- SPI_TEST.ino: Main sketch (setup/loop)
- spi_test_config.h/.cpp: Preset selection and per-device config
- spi_test_spi.h/.cpp: SPI read/write helpers
- spi_test_utils.h: Small print helpers

## Select the DUT

Edit the preset at the top of SPI_TEST.ino:

```cpp
#define SPI_TEST_INIT_PRESET SPI_TEST_PRESET_ADMV1455_MINIMAL
// or
// #define SPI_TEST_INIT_PRESET SPI_TEST_PRESET_ADMV1355_MINIMAL
// #define SPI_TEST_INIT_PRESET SPI_TEST_PRESET_AD9082_MINIMAL
```

## What It Does

- Enables SPI mode on the QuikEval connector
- Sets up SPI with safe defaults (1 MHz, Mode 0)
- Enables SDO for readback
- Reads a small set of safe registers
- Performs a safe write/readback test
  - ADMV1455/ADMV1355: scratchpad register
  - AD9082: SPI interface config register

## Build and Run

1. Open SPI_TEST.ino in Arduino IDE
2. Select Board: Arduino Uno (Linduino One)
3. Select the correct COM port
4. Upload
5. Open Serial Monitor at 115200 baud

## Expected Output (Example)

```
SPI_TEST: Minimal SPI Bring-up
Preset: ADMV1455_MINIMAL
Preset applied: ADMV1455_MINIMAL
SPI Hz: 1000000
SPI Mode: 0
CHIP_ADDR: 0
CS pin: 10
...
Reading a few registers:
  reg 0x000 = 0x..
  reg 0x005 = 0x..
...
Register readback status: VALID
Write/readback test on scratchpad register:
  reg 0x00A write 0x5A read 0x5A
  reg 0x00A write 0xA5 read 0xA5
Scratchpad write/readback: VALID (if no [MISMATCH])
Setup complete.
```

## Preset Examples

### ADMV1455_MINIMAL

```
SPI_TEST: Minimal SPI Bring-up
Preset: ADMV1455_MINIMAL
Preset applied: ADMV1455_MINIMAL
SPI Hz: 1000000
SPI Mode: 0
CHIP_ADDR: 0
CS pin: 10
...
Reading a few registers:
  reg 0x000 = 0x..
  reg 0x005 = 0x..
  reg 0x012 = 0x..
  reg 0x040 = 0x..
  reg 0x050 = 0x..
Register readback status: VALID
Write/readback test on scratchpad register:
  reg 0x00A write 0x5A read 0x5A
  reg 0x00A write 0xA5 read 0xA5
Scratchpad write/readback: VALID (if no [MISMATCH])
Setup complete.
```

### ADMV1355_MINIMAL

```
SPI_TEST: Minimal SPI Bring-up
Preset: ADMV1355_MINIMAL
Preset applied: ADMV1355_MINIMAL
SPI Hz: 1000000
SPI Mode: 0
CHIP_ADDR: 0
CS pin: 10
...
Reading a few registers:
  reg 0x000 = 0x..
  reg 0x001 = 0x..
  reg 0x003 = 0x..
  reg 0x004 = 0x..
  reg 0x005 = 0x..
Register readback status: VALID
Write/readback test on scratchpad register:
  reg 0x00A write 0x5A read 0x5A
  reg 0x00A write 0xA5 read 0xA5
Scratchpad write/readback: VALID (if no [MISMATCH])
Setup complete.
```

### AD9082_MINIMAL

```
SPI_TEST: Minimal SPI Bring-up
Preset: AD9082_MINIMAL
Preset applied: AD9082_MINIMAL
SPI Hz: 1000000
SPI Mode: 0
CHIP_ADDR: 0
CS pin: 10
...
Reading a few registers:
  reg 0x0000 = 0x..
  reg 0x0001 = 0x..
  reg 0x0002 = 0x..
  reg 0x0003 = 0x..
Register readback status: VALID
Write/readback test on SPI config register:
  reg 0x0000 write 0x.. read 0x..
SPI config write/readback status: VALID
Setup complete.
```

## Notes

- If reads return 0xFF or 0x00 consistently, check power, MUX pin, and SPI wiring.
- This project is for connectivity only. Use the part-specific projects for full configuration and bring-up.
