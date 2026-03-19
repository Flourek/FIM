#include "buffer.h"
#include "cursor.h"
#include "helpers.h"
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void niNewline() {
  bufferNewLine(cursor);
  curMoveRelative(0, 1);
  curMove(0, cursor.row);
}

void nReplace(char ch) {
  bufferReplaceChar(cursor, ch);
}

void niLineEnd() {
  curMove(bufferLineLength(cursor.row), cursor.row);
}

void niLineStart() {
  curMove(0, cursor.row);
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

void iInsertCharacter(char input) {
  bufferInsertChar(cursor, input);
  curMoveRelative(1, 0);
}

void nFirstGraph() {
  Pos pos = bufferTraverseFrom(cursor, bufferIsCharGraph, false);
  log("%s", "first non whitespace");
  curMove(pos.col, pos.row);
}

void nWordPrev() {
  curWordPrev();
}

void nWordEnd() {
  curWordEnd();
}

void nWordNext() {
  curWordNextStart();
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
    curMove(len, cursor.row);
    return;
  }
  curMoveRelative(-1, 0);
  bufferDeleteChar(cursor);
}
