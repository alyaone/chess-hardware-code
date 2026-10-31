#pragma once
#include <Arduino.h>
#include "include/define.h"

// Semua logika papan: data papan, setup bidak, deteksi gerakan.
// Tidak menyentuh pin dan tidak mengirim apa pun ke RPi.

// ----- Data bersama -----
// Huruf FEN: K Q R B N P = putih, k q r b n p = hitam, ' ' = kosong
extern char    board[NUM_SQUARES];
extern bool    setupMode;
extern uint8_t currentTurn;

void        boardClear();
int         boardPieceCount();
bool        isTurnPiece(char p);
const char* pieceName(char p);
char        pieceFromCode(const String& code);   // "KW" -> 'K', "QB" -> 'q', salah -> 0
void        squareName(uint8_t sq, char* out);   // out minimal 3 byte, hasil "E2"

// ----- Setup bidak (mode SETUP) -----
void setupRestart();                             // perintah SETUP
bool setupStart(uint8_t turn);                   // perintah START; true kalau pindah ke mode MAIN
void setupOnScan(uint64_t prev, uint64_t cur);   // panggil tiap scan selama setupMode
bool setupAssignPiece(char piece);               // dari kode serial ATAU tombol; true kalau dipakai
bool setupIsWaiting();                           // lagi menunggu kode bidak?

// ----- Deteksi gerakan (mode MAIN) -----
struct Move {
  int8_t from;       // kotak asal
  int8_t to;         // kotak tujuan
  int8_t captured;   // kotak bidak lawan yang dimakan, -1 kalau bukan capture
};

// prev/cur = bitmap sebelum & sesudah scan. true kalau satu gerakan sah selesai
// (board[] & currentTurn sudah di-update).
bool moveDetectorUpdate(uint64_t prev, uint64_t cur, Move& out);
