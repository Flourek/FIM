#ifndef CURSOR_H
#define CURSOR_H

typedef struct {
    int x, y;
} Cursor;
extern Cursor cursor;

void cur_move_relative(int x, int y);
void cur_move(int x, int y);
void cur_clamp();
#endif