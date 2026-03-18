#include "cursor.h"
#include "buffer.h"

Cursor cursor = {0, 0};

void curClamp() {
  int bottomclamp = bufferGet()->line_count - 1;
  if (bottomclamp < 0)
    bottomclamp = 0;

  if (cursor.y >= bottomclamp)
    cursor.y = bottomclamp;
  if (cursor.y < 0)
    cursor.y = 0;

  int rightclamp = strlen(bufferGet()->lines[cursor.y]) - 1;
  if (rightclamp < 0)
    rightclamp = 0;

  if (cursor.x < 0)
    cursor.x = 0;
  if (cursor.x > rightclamp)
    cursor.x = rightclamp;
}

void curInsertMoveRelative(int x, int y) {
  cursor.x += x;
  cursor.y += y;
}

void curMoveRelative(int x, int y) {
  cursor.x += x;
  cursor.y += y;
  curClamp();
}

void curMove(int x, int y) {
  cursor.x = x;
  cursor.y = y;
  curClamp();
}