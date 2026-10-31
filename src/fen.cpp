#include "include/fen.h"
#include "include/board_logic.h"
#include "include/define.h"

String fenGenerate() {
  String fen = "";
  for (int rank = 7; rank >= 0; rank--) {        // rank 8 -> 1
    int emptyCount = 0;
    for (int file = 0; file < 8; file++) {
      char p = board[rank * 8 + file];
      if (p == ' ') {
        emptyCount++;
      } else {
        if (emptyCount > 0) { fen += String(emptyCount); emptyCount = 0; }
        fen += p;
      }
    }
    if (emptyCount > 0) fen += String(emptyCount);
    if (rank > 0) fen += "/";
  }
  fen += (currentTurn == TURN_BLACK) ? " b - - 0 1" : " w - - 0 1";
  return fen;
}
