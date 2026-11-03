#include <Arduino.h>
#include "settings.h"
#include "hardware.h"
#include "rpi_link.h"

// ESP32 cuma "mata": baca papan & tombol, lapor ke GUI. Semua aturan catur ada di GUI (python-chess).

static uint64_t  lastSent   = 0;
static bool      sentOnce   = false;
static uint8_t   scansDone  = 0;
static SwitchPos lastSwitch = SW_OFF;

void setup() {
  rpiBegin();
  hardwareBegin();
  rpiSendReady();
}

void loop() {
  // ----- Perintah dari GUI -----
  String line;
  if (rpiReceive(line)) {
    line.trim();
    line.toUpperCase();
    if (line == "SYNC") sentOnce = false;   // kirim ulang bitmap di scan berikutnya
  }

  // ----- Tombol & switch -----
  controlsUpdate();
  char btn = controlsPieceButton();
  if (btn) rpiSendButton(btn);

  SwitchPos sw = controlsSwitch();
  if (sw != lastSwitch) {
    lastSwitch = sw;
    rpiSendSwitch(sw);
  }

  // ----- Papan: kirim bitmap tiap ada perubahan stabil -----
  if (sensorsScan()) {
    if (scansDone <= STABLE_READS) scansDone++;      // tunggu debounce pertama selesai
    if (scansDone > STABLE_READS) {
      uint64_t occ = sensorsOccupied();
      if (!sentOnce || occ != lastSent) {
        rpiSendOcc(occ);
        lastSent = occ;
        sentOnce = true;
      }
    }
  }

  delay(2);
}
