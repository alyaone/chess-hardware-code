#include "include/board_logic.h"

// =====================================================================
//  DATA PAPAN
// =====================================================================
char    board[NUM_SQUARES];
bool    setupMode   = true;
uint8_t currentTurn = TURN_WHITE;

void boardClear() {
  for (uint8_t i = 0; i < NUM_SQUARES; i++) board[i] = ' ';
}

int boardPieceCount() {
  int n = 0;
  for (uint8_t i = 0; i < NUM_SQUARES; i++) if (board[i] != ' ') n++;
  return n;
}

// huruf kecil = hitam
bool isTurnPiece(char p) {
  if (p == ' ') return false;
  bool isBlack = (p >= 'a' && p <= 'z');
  return (currentTurn == TURN_BLACK) ? isBlack : !isBlack;
}

const char* pieceName(char p) {
  switch (p) {
    case 'K': return "King White";   case 'k': return "King Black";
    case 'Q': return "Queen White";  case 'q': return "Queen Black";
    case 'R': return "Rook White";   case 'r': return "Rook Black";
    case 'B': return "Bishop White"; case 'b': return "Bishop Black";
    case 'N': return "Knight White"; case 'n': return "Knight Black";
    case 'P': return "Pawn White";   case 'p': return "Pawn Black";
    default:  return "-";
  }
}

// Kode 2 huruf: jenis (K Q R B N P) + warna (W / B)
char pieceFromCode(const String& code) {
  if (code.length() != 2) return 0;
  char type  = code.charAt(0);
  char color = code.charAt(1);
  if (!strchr("KQRBNP", type)) return 0;
  if (color == 'W') return type;
  if (color == 'B') return type + ('a' - 'A');
  return 0;
}

void squareName(uint8_t sq, char* out) {
  out[0] = 'A' + (sq % 8);
  out[1] = '1' + (sq / 8);
  out[2] = 0;
}

// =====================================================================
//  DETEKSI GERAKAN
// =====================================================================
static int8_t liftedSquare   = -1;
static int8_t capturedSquare = -1;
static char   liftedPiece    = ' ';

static void resetLift() {
  liftedSquare   = -1;
  capturedSquare = -1;
  liftedPiece    = ' ';
}

bool moveDetectorUpdate(uint64_t prev, uint64_t cur, Move& out) {
  uint64_t lifted = prev & ~cur;
  uint64_t placed = cur & ~prev;
  char name[3];

  // ----- Tahap 1: bidak DIANGKAT -----
  for (uint8_t sq = 0; sq < NUM_SQUARES; sq++) {
    if (!(lifted & (1ULL << sq))) continue;
    squareName(sq, name);

    if (liftedSquare < 0) {
      if (board[sq] == ' ') {
        // kotak tidak punya bidak terdaftar, abaikan
      } else if (ENFORCE_TURN && !isTurnPiece(board[sq])) {
        Serial.print("Bukan giliran bidak di ");
        Serial.println(name);
      } else {
        liftedSquare = sq;
        liftedPiece  = board[sq];
        Serial.print("Diangkat: ");
        Serial.print(pieceName(liftedPiece));
        Serial.print(" dari ");
        Serial.println(name);
      }
    } else if (sq != liftedSquare && capturedSquare < 0) {
      capturedSquare = sq;   // bidak lawan yang dimakan
      Serial.print("Bidak lawan diangkat dari ");
      Serial.println(name);
    }
  }

  // ----- Tahap 2: bidak DILETAKKAN -----
  for (uint8_t sq = 0; sq < NUM_SQUARES; sq++) {
    if (!(placed & (1ULL << sq))) continue;
    if (liftedSquare < 0) continue;            // taruh tanpa angkat, abaikan

    if (sq == liftedSquare) {                  // balik ke kotak asal -> batal
      Serial.println("Gerakan dibatalkan.");
      resetLift();
      continue;
    }
    if (capturedSquare >= 0 && sq != capturedSquare) continue;  // capture: tunggu di kotak lawan

    // Gerakan sah
    board[sq] = liftedPiece;
    board[liftedSquare] = ' ';
    out.from     = liftedSquare;
    out.to       = sq;
    out.captured = capturedSquare;

    currentTurn = (currentTurn == TURN_BLACK) ? TURN_WHITE : TURN_BLACK;
    resetLift();
    return true;
  }
  return false;
}

// =====================================================================
//  SETUP BIDAK
// =====================================================================
static bool     waitingForPiece = false;
static int8_t   pendingSquare   = -1;
static uint32_t lastPromptAt[NUM_SQUARES];

static void promptSquare(uint8_t sq) {
  char name[3];
  squareName(sq, name);
  Serial.print("Set square [");
  Serial.print(name);
  Serial.println("] as (K/Q/R/B/N/P + W/B, mis. KW, QB) atau tekan tombol bidak:");
}

bool setupIsWaiting() { return waitingForPiece; }

void setupRestart() {
  setupMode = true;
  waitingForPiece = false;
  pendingSquare = -1;
  resetLift();
  boardClear();
  for (uint8_t i = 0; i < NUM_SQUARES; i++) lastPromptAt[i] = 0;
  Serial.println("MODE SETUP: pasang bidak dan beri kode.");
}

bool setupStart(uint8_t turn) {
  if (waitingForPiece) {
    Serial.println("Selesaikan dulu kode bidak yang sedang ditanya.");
    return false;
  }
  if (boardPieceCount() == 0) {
    Serial.println("Belum ada bidak yang di-setup.");
    return false;
  }
  setupMode = false;
  currentTurn = turn;
  resetLift();
  Serial.println("=== SETUP SELESAI ===");
  return true;
}

bool setupAssignPiece(char piece) {
  if (!setupMode || pendingSquare < 0) return false;

  board[pendingSquare] = piece;
  char name[3];
  squareName(pendingSquare, name);
  Serial.print("Square [");
  Serial.print(name);
  Serial.print("] saved as [");
  Serial.print(pieceName(piece));
  Serial.println("]");

  waitingForPiece = false;
  pendingSquare = -1;
  return true;
}

void setupOnScan(uint64_t prev, uint64_t cur) {
  uint64_t lifted = prev & ~cur;
  uint64_t placed = cur & ~prev;

  // Bidak diangkat saat setup -> kosongkan kotaknya (diam)
  for (uint8_t sq = 0; sq < NUM_SQUARES; sq++) {
    if (lifted & (1ULL << sq)) {
      board[sq] = ' ';
      if (pendingSquare == sq) { waitingForPiece = false; pendingSquare = -1; }
    }
  }

  // Prompt hanya untuk bidak yang baru ditaruh (tepi naik), satu per satu
  if (!waitingForPiece) {
    for (uint8_t sq = 0; sq < NUM_SQUARES; sq++) {
      if ((placed & (1ULL << sq)) && board[sq] == ' ' &&
          millis() - lastPromptAt[sq] > PROMPT_COOLDOWN_MS) {
        pendingSquare = sq;
        waitingForPiece = true;
        lastPromptAt[sq] = millis();
        promptSquare(sq);
        break;
      }
    }
  }
}
