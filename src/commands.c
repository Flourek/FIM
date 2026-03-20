#include "buffer.h"
#include "cursor.h"
#include "helpers.h"
#include "motion.h"
#include "render.h"
#include "state.h"
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void niNewline() {
  bufferNewLine(cursor);
  curMoveRelative(0, 1);
  curMove((Pos){cursor.row, 0});
}

void nReplace(char ch) {
  bufferReplaceChar(cursor, ch);
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

void iInsertCharacter(char input) {
  bufferInsertChar(cursor, input);
  curMoveRelative(1, 0);
}

void nFirstGraph() {
  Pos pos = bufferTraverse(cursor, bufferEnd(), bufferIsCharGraph);
  log("%s", "first non whitespace");
  curMove(pos);
}

void nWordPrev() {
  Pos prevPos = {cursor.row, cursor.col - 1};

  if (bufferIsCharBlank(prevPos))
    curPrevWordEnd();

  curWordStart();
}

void nWordEnd() {
  Pos nextPos = {cursor.row, cursor.col + 1};

  if (bufferIsCharBlank(nextPos))
    curNextWordStart();

  curWordEnd();
}

void nWordNext() {
  curNextWordStart();
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
  curMoveRelative(-1, 0);
  bufferDeleteChar(cursor);
}
