#pragma once
#include <Arduino.h>

constexpr uint8_t  NUM_SQUARES      = 64;
constexpr uint32_t SERIAL_BAUD      = 115200;
constexpr uint32_t SCAN_INTERVAL_MS = 20;   // 1 scan penuh (64 sensor) ~2 ms
constexpr uint8_t  STABLE_READS     = 3;    // scan berturut-turut yang sama sebelum dianggap berubah
