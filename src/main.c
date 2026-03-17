#include "buffer.h"
#include "cursor.h"
#include "math.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_CURSOR_RIGHT 'l'
#define N_CURSOR_LEFT 'h'
#define N_CURSOR_UP 'k'
#define N_CURSOR_DOWN 'j'
#define N_QUIT 'q'
#define N_ENTER_INSERT 'i'
#define I_LEAVE_INSERT 27
#define I_NEWLINE 'K'
#define I_LINE_START '0'
#define I_LINE_END '$'

typedef enum {
  MODE_NORMAL,
  MODE_INSERT
} Mode;
const char *ModeNames[] = {"Normal", "Insert", "Visual", "Visual_line"};

Mode mode = MODE_NORMAL;

void normalMode(char input) {

  switch (input) {
  case N_ENTER_INSERT:
    mode = MODE_INSERT;
    break;
  case I_NEWLINE:
    N_newline();
    break;
  case I_LINE_START:
    cur_move(0, 0);
    break;
  case N_CURSOR_LEFT:
    cur_move_relative(-1, 0);
    break;
  case N_CURSOR_RIGHT:
    cur_move_relative(1, 0);
    break;
  case N_CURSOR_UP:
    cur_move_relative(0, -1);
    break;
  case N_CURSOR_DOWN:
    cur_move_relative(0, 1);
    break;
  }
}

void insertMode(char input) {
  switch (input) {
  case I_LEAVE_INSERT:
    mode = MODE_NORMAL;
    return;
  }

  Buffer_insert_char(cursor.y, cursor.x, input);
  cur_move_relative(1, 0);
}

void render(Buffer *buf, const Cursor cursor) {
  erase();

  for (int i = 0; i < buf->line_count; i++) {
    mvprintw(i, 0, "%s", buf->lines[i]);
  }

  attron(A_BOLD);
  mvprintw(LINES - 1, 0, "DEBUG: x=%d y=%d mode=%s | LINES: %d", cursor.x,
           cursor.y, ModeNames[mode], Buffer_get()->line_count);
  clrtoeol();
  attroff(A_BOLD);

  move(cursor.y, cursor.x);

  refresh(); // draw to screen
}

int main() {
  initscr();            // start ncurses
  cbreak();             // disable line buffering
  noecho();             // don't print typed chars
  keypad(stdscr, TRUE); // enable arrow keys

  Buffer *buf = Buffer_new();
  Buffer_insert_line(0, "chuj");
  render(buf, cursor);

  while (true) {
    char input = getch();

    switch (mode) {
    case MODE_INSERT:
      /* code */
      insertMode(input);
      break;

    case MODE_NORMAL:
      normalMode(input);
      break;

    default:
      break;
    }

    render(buf, cursor);
  }

  endwin(); // restore terminal
  return 0;
}
