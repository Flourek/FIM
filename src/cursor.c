#include "cursor.h"
#include "buffer.h"
#include "helpers.h"
#include "state.h"
#include <string.h>

Cursor cursor = {0, 0};

void curClamp() {
  int bottomclamp = bufferGet()->line_count - 1;
  if (bottomclamp < 0)
    bottomclamp = 0;

  if (cursor.row >= bottomclamp)
    cursor.row = bottomclamp;
  if (cursor.row < 0)
    cursor.row = 0;

  int padding = state.mode == MODE_INSERT ? 0 : 1;
  char *line = bufferGetLine((Pos){cursor.row, 0});
  int rightclamp = (int)strlen(line ? line : "") - padding;

  if (rightclamp < 0)
    rightclamp = 0;

  if (cursor.col < 0)
    cursor.col = 0;
  if (cursor.col > rightclamp)
    cursor.col = rightclamp;
}

void curMoveRelative(int x, int y) {
  cursor.col += x;
  cursor.row += y;
  curClamp();
}

void curMove(int x, int y) {
  cursor.col = x;
  cursor.row = y;
  curClamp();
}

void curWordNext() {
  Pos pos = bufferTraverseFrom(cursor, bufferIsCharBlank, false);
  pos = bufferTraverseFrom(pos, bufferIsCharGraph, false);
  curMove(pos.col, pos.row);
}

void curWordPrev() {
  Pos prevPos = {cursor.row, cursor.col - 1};
  Pos pos = cursor;

  if (bufferIsCharBlank(prevPos)) {
    pos = bufferTraverseFrom(pos, bufferIsCharBlank, true);
    pos = bufferTraverseFrom(pos, bufferIsCharGraph, true);
  }

  // if (cursorAtBegin) curWordNext();
  pos = bufferTraverseFrom(pos, bufferIsCharBlank, true);
  pos = bufferTraverseFrom(pos, bufferIsCharGraph, false);

  curMove(pos.col, pos.row);
}

void curWordEnd() {
  Pos nextPos = {cursor.row, cursor.col + 1};

  if (bufferIsCharBlank(nextPos))
    curWordNext();

  Pos blankPos = bufferTraverseFrom(cursor, bufferIsCharBlank, false);

  curMove(blankPos.col - 1, blankPos.row);
  log("%i %i", cursorAtEnd, blankPos.col);
}
