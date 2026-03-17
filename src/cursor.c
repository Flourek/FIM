#include "cursor.h"
#include "buffer.h"

Cursor cursor = {0, 0};

void cur_clamp() {
  if (cursor.x < 0)
    cursor.x = 0;
  if (cursor.y < 0)
    cursor.y = 0;
  if (cursor.y >= Buffer_get()->line_count - 1)
    cursor.y = Buffer_get()->line_count - 1;
  if (cursor.y < 0)
    cursor.y = 0;
}

void cur_move_relative(int x, int y) {
  cursor.x += x;
  cursor.y += y;
  cur_clamp();
}

void cur_move(int x, int y) {
  cursor.x = x;
  cursor.y = y;
  cur_clamp();
}