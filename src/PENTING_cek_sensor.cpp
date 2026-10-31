//////////
//UBAH JADI MAIN.CPP DULU YAAAA

#include <Arduino.h>

const uint8_t PIN_S[4]   = {25, 26, 27, 14};   // S0..S3
const uint8_t PIN_SIG[4] = {33, 32, 35, 34};   // SIG MUX1..MUX4

const char* const SQUARE_NAME[4][16] = {
  // C0   C1   C2   C3   C4   C5   C6   C7   C8   C9   C10  C11  C12  C13  C14  C15
  {"D1","D2","D3","D4","C1","C2","C3","C4","B1","B2","B3","B4","A4","A3","A2","A1"},  // MUX1
  {"H1","H2","H3","H4","G1","G2","G3","G4","F1","F2","F3","F4","E1","E2","E3","E4"},  // MUX2
  {"E8","E7","E6","E5","F8","F7","F6","F5","G8","G7","G6","G5","H5","H6","H7","H8"},  // MUX3
  {"A8","A7","A6","A5","B8","B7","B6","B5","C8","C7","C6","C5","D8","D7","D6","D5"}   // MUX4
};

const uint8_t  STABLE_READS     = 3;
const uint16_t SCAN_INTERVAL_MS = 20;

bool    stableState[4][16];
bool    rawPrev[4][16];
uint8_t sameCount[4][16];

void selectChannel(uint8_t ch) {
  for (uint8_t i = 0; i < 4; i++) digitalWrite(PIN_S[i], (ch >> i) & 1);
}

void setup() {
  Serial.begin(115200);
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(PIN_S[i], OUTPUT);
    pinMode(PIN_SIG[i], INPUT);
  }
  Serial.println("UPLOADED. SISTEM SIAP DIUJI");
}

void loop() {
  static uint32_t lastScan = 0;
  if (millis() - lastScan < SCAN_INTERVAL_MS) return;
  lastScan = millis();

  for (uint8_t ch = 0; ch < 16; ch++) {
    selectChannel(ch);
    delayMicroseconds(100);

    for (uint8_t m = 0; m < 4; m++) {
      bool r = (digitalRead(PIN_SIG[m]) == LOW);   // aktif LOW = ada magnet

      if (r == rawPrev[m][ch]) { if (sameCount[m][ch] < 255) sameCount[m][ch]++; }
      else                     { sameCount[m][ch] = 1; }
      rawPrev[m][ch] = r;

      if (sameCount[m][ch] >= STABLE_READS && r != stableState[m][ch]) {
        stableState[m][ch] = r;
        Serial.print("SQUARE[");
        Serial.print(SQUARE_NAME[m][ch]);
        Serial.print("] DI MUX [");
        Serial.print(m + 1);
        Serial.print("] PADA CH [");
        Serial.print(ch);
        Serial.println(r ? "] ADA MAGNET" : "] MAGNET DIANGKAT");
      }
    }
  }
}