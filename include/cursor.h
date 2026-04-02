#ifndef CURSOR_H
#define CURSOR_H

#include "buffer.h"

typedef Pos Cursor;

extern Cursor cursor;

void curMoveRelative(int x, int y);
void curMove(Pos pos);
void curInsertMove(Pos pos);
void curReset();
void curClamp();
#endif