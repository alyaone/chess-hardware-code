#pragma once
#include <Arduino.h>

// ===== Select line (dipakai bareng SEMUA MUX) =====
constexpr uint8_t PIN_S0 = 25;
constexpr uint8_t PIN_S1 = 26;
constexpr uint8_t PIN_S2 = 27;
constexpr uint8_t PIN_S3 = 14;

// ===== SIG tiap MUX papan (MUX1..MUX4) =====
constexpr uint8_t NUM_BOARD_MUX = 4;
constexpr uint8_t PIN_SIG_BOARD[NUM_BOARD_MUX] = {33, 32, 35, 34};  // 34 & 35 input-only (KY-003 sudah ada pull-up)

// ===== Tombol bidak & switch (via MUX kontrol) =====
// TODO: pin & wiring bagian ini belum kamu kasih, aku isi sementara. Sesuaikan!
constexpr uint8_t PIN_SIG_CTRL         = 4;    // SIG MUX kontrol (pakai pull-up internal)
constexpr uint8_t CTRL_BUTTON_COUNT    = 12;   // channel 0..11
constexpr uint8_t CTRL_CH_SWITCH_WHITE = 12;
constexpr uint8_t CTRL_CH_SWITCH_BLACK = 13;   // WHITE & BLACK sama-sama HIGH = OFF

// ===== Peta channel -> kotak: SQUARE_NAME[mux][channel] =====
constexpr const char* const SQUARE_NAME[NUM_BOARD_MUX][16] = {
  // C0   C1   C2   C3   C4   C5   C6   C7   C8   C9   C10  C11  C12  C13  C14  C15
  {"D1","D2","D3","D4","C1","C2","C3","C4","B1","B2","B3","B4","A4","A3","A2","A1"},  // MUX1
  {"H1","H2","H3","H4","G1","G2","G3","G4","F1","F2","F3","F4","E1","E2","E3","E4"},  // MUX2
  {"E8","E7","E6","E5","F8","F7","F6","F5","G8","G7","G6","G5","H5","H6","H7","H8"},  // MUX3
  {"A8","A7","A6","A5","B8","B7","B6","B5","C8","C7","C6","C5","D8","D7","D6","D5"}   // MUX4
};
