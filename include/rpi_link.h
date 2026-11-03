#pragma once
#include <Arduino.h>
#include "hardware.h"

// Protokol ESP32 -> GUI (satu baris per pesan, 115200 baud):
//   READY                  saat boot
//   OCC:<16 hex>           bitmap 64-bit kotak terisi (stabil), dikirim tiap ada perubahan
//   BTN:<huruf>            tombol bidak ditekan (K Q R B N P = putih, k q r b n p = hitam)
//   SW:WHITE|BLACK|OFF     posisi switch berubah
// Protokol GUI -> ESP32:
//   SYNC                   minta kirim ulang OCC

void rpiBegin();
bool rpiReceive(String& line);      // non-blocking; true kalau 1 baris lengkap
void rpiSendReady();
void rpiSendOcc(uint64_t occ);
void rpiSendButton(char piece);
void rpiSendSwitch(SwitchPos pos);
