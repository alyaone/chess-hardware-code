#pragma once
#include <Arduino.h>

// Semua yang menyentuh pin: MUX, 64 sensor papan, 12 tombol bidak, switch.

enum SwitchPos : uint8_t { SW_OFF, SW_WHITE, SW_BLACK };

void      hardwareBegin();

// ----- Sensor papan -----
bool      sensorsScan();           // true kalau ada scan baru (sesuai SCAN_INTERVAL_MS)
uint64_t  sensorsOccupied();       // bitmap stabil: bit n = kotak n terisi (A1=0, B1=1, ... H1=7, A2=8, ... H8=63)

// ----- Tombol & switch -----
void      controlsUpdate();
char      controlsPieceButton();   // huruf FEN tombol yang baru ditekan (K Q R B N P / k q r b n p), 0 kalau tidak ada
SwitchPos controlsSwitch();
