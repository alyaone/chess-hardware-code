#pragma once
#include <Arduino.h>

// Semua yang menyentuh pin: MUX, 64 sensor papan, 12 tombol bidak, switch.

enum SwitchPos : uint8_t { SW_OFF, SW_WHITE, SW_BLACK };

void      hardwareBegin();          // atur semua pin (mux, sensor, kontrol)

// ----- Sensor papan -----
bool      sensorsScan();            // true kalau ada scan baru (sesuai SCAN_INTERVAL_MS)
uint64_t  sensorsOccupied();        // bitmap hasil debounce, bit n = kotak n terisi

// ----- Tombol & switch -----
void      controlsUpdate();         // panggil tiap loop
char      controlsPieceButton();    // huruf FEN tombol yang baru ditekan, 0 kalau tidak ada (sekali baca)
SwitchPos controlsSwitch();
