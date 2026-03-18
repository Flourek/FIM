#ifndef CURSOR_H
#define CURSOR_H

typedef struct {
  int x, y;
} Cursor;
extern Cursor cursor;

void curMoveRelative(int x, int y);
void curInsertMoveRelative(int x, int y);
void curMove(int x, int y);
void curClamp();
#endif