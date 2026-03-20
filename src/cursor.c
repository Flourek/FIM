#include "cursor.h"
#include "buffer.h"
#include "helpers.h"
#include "motion.h"
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

void curMove(Pos pos) {
  cursor.col = pos.col;
  cursor.row = pos.row;
  curClamp();
}

void curNextWordStart() {
  Range range = motionWordNextStart(cursor);
  curMove(range.end);
}

void curWordStart() {
  Range range = motionWordStart(cursor);
  curMove(range.end);
}

void curPrevWordEnd() {
  Range range = motionPrevWordEnd(cursor);
  curMove(range.end);
}

void curWordEnd() {
  Range range = motionWordEnd(cursor);
  curMove(range.end);
}
