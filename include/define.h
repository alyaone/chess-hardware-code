#pragma once
#include <Arduino.h>

// ===== Select line (dipakai bareng SEMUA MUX: 4 papan + 1 kontrol) =====
constexpr uint8_t PIN_S0 = 12;   // GPIO12 = strapping pin; kalau ESP32 gagal boot, pindahkan
constexpr uint8_t PIN_S1 = 14;
constexpr uint8_t PIN_S2 = 27;
constexpr uint8_t PIN_S3 = 26;

// ===== SIG tiap MUX papan (MUX1..MUX4) =====
constexpr uint8_t NUM_BOARD_MUX = 4;
constexpr uint8_t PIN_SIG_BOARD[NUM_BOARD_MUX] = {32, 33, 34, 35};  // 34 & 35 input-only (KY-003 sudah punya pull-up)

// ===== SIG MUX kontrol (12 tombol bidak + switch) =====
constexpr uint8_t PIN_SIG_CTRL = 25;  // TODO: sesuaikan pinout final

// ===== Peta sensor -> index kotak =====
// Index kotak: A1=0, B1=1, ... H1=7, A2=8, ... H8=63
// Default: MUX1=rank 1-2, MUX2=rank 3-4, MUX3=rank 5-6, MUX4=rank 7-8, channel urut.
// TODO: sesuaikan kalau wiring PCB beda (ubah rumus ini saja).
constexpr uint8_t squareIndex(uint8_t mux, uint8_t channel) { return mux * 16 + channel; }

// ===== Channel MUX kontrol =====
constexpr uint8_t CTRL_BUTTON_COUNT    = 12;  // channel 0..11
constexpr uint8_t CTRL_CH_SWITCH_WHITE = 12;  // TODO: sesuaikan wiring switch 3 posisi
constexpr uint8_t CTRL_CH_SWITCH_BLACK = 13;  // WHITE & BLACK sama-sama HIGH = OFF

#pragma once
#include <Arduino.h>

constexpr uint8_t  NUM_SQUARES         = 64;
constexpr uint32_t SERIAL_BAUD         = 115200;

// Scan & debounce
constexpr uint32_t SCAN_INTERVAL_MS    = 30;    // 1 scan penuh (64 sensor) ~2 ms
constexpr uint8_t  STABLE_READS        = 3;     // scan berturut-turut yang sama
constexpr uint32_t PROMPT_COOLDOWN_MS  = 2000;  // jeda prompt untuk kotak yang sama

// Giliran
constexpr uint8_t  TURN_BLACK          = 1;
constexpr uint8_t  TURN_WHITE          = 2;
constexpr bool     ENFORCE_TURN        = false; // true = hanya bidak giliran saat ini yang boleh diangkat

constexpr bool     BOOT_MESSAGE        = false;
