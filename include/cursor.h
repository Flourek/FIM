#ifndef CURSOR_H
#define CURSOR_H

#include "buffer.h"

typedef Pos Cursor;

extern Cursor cursor;

void curMoveRelative(int x, int y);
void curInsertMoveRelative(int x, int y);
void curMove(Pos pos);
void curInsertMove(Pos pos);
void curClamp();
void curNextWordStart();
void curWordEnd();
void curWordStart();
void curPrevWordEnd();
#endif