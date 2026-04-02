#include "cursor.h"
#include "buffer.h"
#include "helpers.h"
#include "motion.h"
#include "state.h"
#include "utf8.h"
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
  char *line = bufferGetLine(cursor.row);
  int rightclamp = (int)strlen(line ? line : "") - padding;
  // Note: rightclamp is byte length. Clamping to byte length is correct for storage.

  if (rightclamp < 0)
    rightclamp = 0;

  if (cursor.col < 0)
    cursor.col = 0;
  if (cursor.col > rightclamp)
    cursor.col = rightclamp;
}

void curMoveRelative(int x, int y) {
  // Handle vertical movement first (simple row update)
  cursor.row += y;
  cursor.col += x;

  // Handle horizontal movement (UTF-8 aware)
  // char *line = bufferGetLine(cursor.row);

  // if (x > 0) {
  //   for (int i = 0; i < x; i++) {
  //     cursor.col = utf8_next(line, cursor.col);
  //   }
  // } else if (x < 0) {
  //   for (int i = 0; i > x; i--) {
  //     cursor.col = utf8_prev(line, cursor.col);
  //   }
  // }

  curClamp();
}
void curReset() {
  cursor.col = 0;
  cursor.row = 0;
}

void curMove(Pos pos) {
  cursor.col = pos.col;
  cursor.row = pos.row;
  // log("%i %i", cursor.row, cursor.col);
  curClamp();
}