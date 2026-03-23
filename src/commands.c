#include "buffer.h"
#include "cursor.h"
#include "helpers.h"
#include "motion.h"
#include "render.h"
#include "state.h"
#include "utf8.h"
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void niNewline() {
  bufferNewLine(cursor);
  curMoveRelative(0, 1);
  curMove((Pos){cursor.row, 0});
}

void nReplace(wint_t ch) {
  // To replace properly with UTF-8, we delete the current char and insert the new one.
  nDeleteChar();
  bufferInsertChar(cursor, ch);
  cursor.col++;
  // move back to original char
  // Since nDeleteChar keeps cursor at start of next char?
  // Wait, if I delete 3 bytes at pos 0, text shifts left. pos is still 0.
  // I insert 3 bytes. buffer now has 3 bytes at pos 0. cursor moves 0->1->2->3.
  // Final cursor should be on the replaced char? Vim behavior: cursor stays on the char.
  // So I should move cursor back to start.
  curMoveRelative(-1, 0);
}

void nSubstitute() {
  nDeleteChar();
  nInsert();
}

void nDeleteChar() {
  bufferDeleteChar(cursor);
}

void niLineEnd() {
  Range range = motionLineEnd(cursor);
  curMove(range.end);
}

void niLineStart() {
  Range range = motionLineStart(cursor);
  curMove(range.end);
}

void niCursorLeft() {
  curMoveRelative(-1, 0);
}

void niCursorRight() {
  curMoveRelative(1, 0);
}

void niCursorUp() {
  curMoveRelative(0, -1);
}

void niCursorDown() {
  curMoveRelative(0, 1);
}

void nQuit() {
  endwin();
  exit(EXIT_SUCCESS);
}

void nAppend() {
  nInsert();
  niCursorRight();
}

void nInsert() {
  state.mode = MODE_INSERT;
  renderSetCursorStyle(CURSOR_STYLE_BAR);
}

void iLeaveInsert() {
  state.mode = MODE_NORMAL;
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  niCursorLeft();
}

#include "utf8.h"

void iInsertCharacter(wint_t ch) {
  bufferInsertChar(cursor, ch);
  curMoveRelative(1, 0);
}

void nMergeLine() {
  bufferMergeLine(cursor.row, cursor.row + 1, cursor.col);
}

void iBackspace() {
  if (cursor.col == 0 && cursor.row == 0)
    return;

  if (cursor.col <= 0) {
    curMoveRelative(0, -1);
    int len = bufferLineLength(cursor.row);
    bufferMergeLine(cursor.row, cursor.row + 1, cursor.col);
    curMove((Pos){cursor.row, len});
    return;
  }

  // Calculate how many bytes to delete for the previous character
  int endCol = cursor.col;
  curMoveRelative(-1, 0); // Uses utf8_prev
  int startCol = cursor.col;
  int bytesToDelete = endCol - startCol;

  for (int i = 0; i < bytesToDelete; i++) {
    bufferDeleteChar(cursor);
  }
}
