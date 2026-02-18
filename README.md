# SPI_TEST

Minimal SPI connectivity test for Linduino (DC2026) to validate communication with:
- ADMV1455 (register 0x00A)

This sketch is intentionally simple and only verifies SPI wiring, chip select, and basic register read/write behavior. Full device bring-up and programming live in the part-specific projects.

## Hardware

- Linduino One (DC2026)
- ADMV1455 on an evaluation board or custom PCB
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
- spi_test_config.h/.cpp: ADMV1455 config
- spi_test_spi.h/.cpp: SPI read/write helpers
- spi_test_utils.h: Small print helpers

## What It Does

- Enables SPI mode on the QuikEval connector
- Sets up SPI with safe defaults (1 MHz, Mode 0)
- Enables SDO for readback
- Reads register 0x00A
- Performs a safe write/readback test on register 0x00A

## Build and Run

1. Open SPI_TEST.ino in Arduino IDE
2. Select Board: Arduino Uno (Linduino One)
3. Select the correct COM port
4. Upload
5. Open Serial Monitor at 115200 baud

## Expected Output (Example)

```
SPI_TEST: ADMV1455 SPI Bring-up (reg 0x00A)
SPI Hz: 1000000
SPI Mode: 0
CHIP_ADDR: 0
CS pin: 10
...
Reading register 0x00A:
  reg 0x00A = 0x..
Register readback status: VALID
Write/readback test on register 0x00A:
  reg 0x00A write 0x5A read 0x5A
  reg 0x00A write 0xA5 read 0xA5
Register 0x00A write/readback: VALID (if no [MISMATCH])
Setup complete.
```

## Notes

- If reads return 0xFF or 0x00 consistently, check power, MUX pin, and SPI wiring.
- This project is for connectivity only. Use the part-specific projects for full configuration and bring-up.
