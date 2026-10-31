#include <Arduino.h>
#include "config/settings.h"
#include "hardware/hardware_io.h"
#include "hardware/board_logic.h"
#include "comm/fen.h"
#include "comm/rpi_link.h"

static uint64_t prevOcc = 0;

static void processInput(String input) {
  input.trim();
  input.toUpperCase();

  if (input == "START" || input == "START W") {
    if (setupMode && setupStart(TURN_WHITE)) rpiSendSetupData(fenGenerate());   // FEN cuma dikirim SEKALI di sini
    return;
  }
  if (input == "START B") {
    if (setupMode && setupStart(TURN_BLACK)) rpiSendSetupData(fenGenerate());
    return;
  }
  if (input == "SETUP")  { setupRestart(); return; }
  if (input == "FEN")    { rpiSendSetupData(fenGenerate()); return; }
  if (input == "STATUS") { rpiSendStatus(prevOcc); return; }

  // Kode bidak (hanya saat setup & sedang ditanya)
  if (!setupMode || !setupIsWaiting()) return;
  char piece = pieceFromCode(input);
  if (piece) setupAssignPiece(piece);
  else Serial.println("Kode tidak valid. Contoh: KW, QB, RW, BB, NW, PB.");
}

void setup() {
  rpiBegin();
  hardwareBegin();
  boardClear();
}

void loop() {
  String line;
  if (rpiReceive(line)) processInput(line);

  controlsUpdate();
  char btn = controlsPieceButton();
  if (btn && setupMode) setupAssignPiece(btn);
  // controlsSwitch() siap dipakai (SW_WHITE / SW_OFF / SW_BLACK), belum disambungkan ke logika apa pun

  if (sensorsScan()) {
    uint64_t occ = sensorsOccupied();
    if (setupMode) {
      setupOnScan(prevOcc, occ);
    } else {
      Move m;
      if (moveDetectorUpdate(prevOcc, occ, m)) rpiSendMove(m.from, m.to);
    }
    prevOcc = occ;
  }

  delay(10);
}
