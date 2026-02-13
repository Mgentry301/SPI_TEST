#pragma once

#include <Arduino.h>

static inline void print_hex8(uint8_t v) {
  if (v < 0x10) Serial.print('0');
  Serial.print(v, HEX);
}
