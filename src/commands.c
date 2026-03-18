#include "buffer.h"
#include "cursor.h"
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void nNewline() {
  bufferInsertLine(bufferGet()->line_count, NULL);
  curMoveRelative(0, 1);
  curMove(0, cursor.y);
}

void nLineEnd() {
  curMove(bufferLineLength(cursor.y), cursor.y);
}

void nLineStart() {
  curMove(0, cursor.y);
}

void nCursorLeft() {
  curMoveRelative(-1, 0);
}

void nCursorRight() {
  curMoveRelative(1, 0);
}

void nCursorUp() {
  curMoveRelative(0, -1);
}

void nCursorDown() {
  curMoveRelative(0, 1);
}

void nQuit() {
  endwin();
  exit(EXIT_SUCCESS);
}

void iBackspace() {
  bufferDeleteChar(cursor.y, cursor.x - 1);
}
