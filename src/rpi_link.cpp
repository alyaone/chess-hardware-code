#include "rpi_link.h"
#include "settings.h"

static String inputLine = "";

void rpiBegin() {
  Serial.begin(SERIAL_BAUD);
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

void rpiSendReady() {
  Serial.println("READY");
}

void rpiSendOcc(uint64_t occ) {
  char buf[24];
  snprintf(buf, sizeof(buf), "OCC:%08lX%08lX",
           (unsigned long)(occ >> 32), (unsigned long)(occ & 0xFFFFFFFFULL));
  Serial.println(buf);
}

void rpiSendButton(char piece) {
  Serial.print("BTN:");
  Serial.println(piece);
}

void rpiSendSwitch(SwitchPos pos) {
  Serial.print("SW:");
  Serial.println(pos == SW_WHITE ? "WHITE" : (pos == SW_BLACK ? "BLACK" : "OFF"));
}
