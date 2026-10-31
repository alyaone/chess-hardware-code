#include "include/rpi_link.h"
#include "include/board_logic.h"
#include "include/define.h"

static String inputLine = "";

void rpiBegin() {
  Serial.begin(SERIAL_BAUD);
  if (BOOT_MESSAGE) Serial.println("ESP32 siap.");
}

bool rpiReceive(String& line) {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputLine.length() > 0) {
        line = inputLine;
        inputLine = "";
        return true;
      }
    } else {
      inputLine += c;
    }
  }
  return false;
}

void rpiSendFen(const String& fen) {
  Serial.print("FEN:");
  Serial.println(fen);
}

void rpiSendSetupData(const String& fen) {
  rpiSendFen(fen);

  Serial.print("TURN,");
  Serial.println(currentTurn == TURN_BLACK ? "BLACK" : "WHITE");

  // Hanya kotak berisi (bukan 64 entri) supaya baris tidak kepanjangan
  Serial.print("BOARD");
  char name[3];
  for (uint8_t i = 0; i < NUM_SQUARES; i++) {
    if (board[i] == ' ') continue;
    squareName(i, name);
    Serial.print(",");
    Serial.print(name);
    Serial.print("=");
    Serial.print(board[i]);
  }
  Serial.println();
  Serial.println("END_SETUP_DATA");
}

void rpiSendMove(uint8_t from, uint8_t to) {
  char a[3], b[3];
  squareName(from, a);
  squareName(to, b);
  Serial.print("MOVE:");
  Serial.print(a);
  Serial.print("->");
  Serial.println(b);
}

void rpiSendStatus(uint64_t occupied) {
  Serial.print("STATUS mode=");
  Serial.print(setupMode ? "SETUP" : "MAIN");
  Serial.print(" turn=");
  Serial.print(currentTurn == TURN_BLACK ? "BLACK" : "WHITE");
  Serial.print(" sens=");
  for (uint8_t i = 0; i < NUM_SQUARES; i++) Serial.print(((occupied >> i) & 1ULL) ? 1 : 0);
  Serial.print(" board=");
  for (uint8_t i = 0; i < NUM_SQUARES; i++) Serial.print(board[i] == ' ' ? '-' : board[i]);
  Serial.println();
}
