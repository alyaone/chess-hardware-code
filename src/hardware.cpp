#include "hardware.h"
#include "pins.h"
#include "settings.h"

// =====================================================================
//  MUX (internal)
// =====================================================================
static uint8_t squareOf[NUM_BOARD_MUX][16];   // [mux][channel] -> index kotak 0..63

static void muxSelect(uint8_t channel) {
  digitalWrite(PIN_S0, (channel >> 0) & 1);
  digitalWrite(PIN_S1, (channel >> 1) & 1);
  digitalWrite(PIN_S2, (channel >> 2) & 1);
  digitalWrite(PIN_S3, (channel >> 3) & 1);
}

void hardwareBegin() {
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);
  for (uint8_t m = 0; m < NUM_BOARD_MUX; m++) pinMode(PIN_SIG_BOARD[m], INPUT);
  pinMode(PIN_SIG_CTRL, INPUT_PULLUP);

  // "E2" -> file E (4), rank 2 (1) -> index 1*8 + 4 = 12  (sama dengan chess.E2 di python-chess)
  for (uint8_t m = 0; m < NUM_BOARD_MUX; m++) {
    for (uint8_t ch = 0; ch < 16; ch++) {
      const char* n = SQUARE_NAME[m][ch];
      squareOf[m][ch] = (n[1] - '1') * 8 + (n[0] - 'A');
    }
  }
}

// =====================================================================
//  SENSOR PAPAN
// =====================================================================
static uint64_t rawPrev   = 0;
static uint64_t stableMap = 0;
static uint8_t  sameCount[NUM_SQUARES];
static uint32_t lastScan  = 0;

// Pilih 1 channel, baca SIG keempat MUX sekaligus -> 16 langkah untuk 64 kotak
static uint64_t readRaw() {
  uint64_t bits = 0;
  for (uint8_t ch = 0; ch < 16; ch++) {
    muxSelect(ch);
    delayMicroseconds(100);
    for (uint8_t m = 0; m < NUM_BOARD_MUX; m++) {
      if (digitalRead(PIN_SIG_BOARD[m]) == LOW) {   // KY-003 aktif LOW saat ada magnet
        bits |= 1ULL << squareOf[m][ch];
      }
    }
  }
  return bits;
}

bool sensorsScan() {
  if (millis() - lastScan < SCAN_INTERVAL_MS) return false;
  lastScan = millis();

  uint64_t raw = readRaw();
  for (uint8_t sq = 0; sq < NUM_SQUARES; sq++) {
    uint64_t bit = 1ULL << sq;
    bool r = raw & bit;
    bool p = rawPrev & bit;
    if (r == p) { if (sameCount[sq] < 100) sameCount[sq]++; }
    else        { sameCount[sq] = 1; }

    if (sameCount[sq] >= STABLE_READS) {
      if (r) stableMap |= bit; else stableMap &= ~bit;
    }
  }
  rawPrev = raw;
  return true;
}

uint64_t sensorsOccupied() { return stableMap; }

// =====================================================================
//  TOMBOL BIDAK & SWITCH
// =====================================================================
// Urutan channel 0..11 -> bidak. TODO: sesuaikan dengan wiring tombol.
static const char BUTTON_PIECE[CTRL_BUTTON_COUNT] = {'K','Q','R','B','N','P','k','q','r','b','n','p'};

static uint16_t  lastSample  = 0;
static uint16_t  stableState = 0;   // bit n = tombol n sedang ditekan
static char      pendingBtn  = 0;
static SwitchPos switchPos   = SW_OFF;
static uint32_t  lastRead    = 0;

static bool readChannelLow(uint8_t ch) {
  muxSelect(ch);
  delayMicroseconds(50);
  return digitalRead(PIN_SIG_CTRL) == LOW;   // tombol/switch aktif LOW
}

void controlsUpdate() {
  if (millis() - lastRead < 20) return;
  lastRead = millis();

  uint16_t now = 0;
  for (uint8_t i = 0; i < CTRL_BUTTON_COUNT; i++) {
    if (readChannelLow(i)) now |= (1u << i);
  }

  // debounce: state dipakai kalau 2 sample berturut-turut sama
  if (now == lastSample) {
    uint16_t pressed = now & ~stableState;
    stableState = now;
    if (pressed && pendingBtn == 0) {
      for (uint8_t i = 0; i < CTRL_BUTTON_COUNT; i++) {
        if (pressed & (1u << i)) { pendingBtn = BUTTON_PIECE[i]; break; }
      }
    }
  }
  lastSample = now;

  bool w = readChannelLow(CTRL_CH_SWITCH_WHITE);
  bool b = readChannelLow(CTRL_CH_SWITCH_BLACK);
  switchPos = w ? SW_WHITE : (b ? SW_BLACK : SW_OFF);
}

char controlsPieceButton() {
  char c = pendingBtn;
  pendingBtn = 0;
  return c;
}

SwitchPos controlsSwitch() { return switchPos; }
