#pragma once
#include <Arduino.h>

void rpiBegin();
bool rpiReceive(String& line);                 // non-blocking; true kalau 1 baris perintah lengkap
void rpiSendSetupData(const String& fen);      // FEN + TURN + BOARD + END_SETUP_DATA
void rpiSendMove(uint8_t from, uint8_t to);    // MOVE:E2->E4
void rpiSendFen(const String& fen);            // FEN:...
void rpiSendStatus(uint64_t occupied);
