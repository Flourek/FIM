#ifndef CURSOR_H
#define CURSOR_H

#include "buffer.h"

typedef Pos Cursor;

extern Cursor cursor;

void curMoveRelative(int x, int y);
void curInsertMoveRelative(int x, int y);
void curMove(int x, int y);
void curInsertMove(int x, int y);
void curClamp();
void curWordNextStart();
void curWordPrev();
#endif