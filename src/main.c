#include "buffer.h"
#include "commands.h"
#include "cursor.h"
#include "render.h"
#include <math.h>
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
#define I_BACKSPACE KEY_BACKSPACE
#define N_NEWLINE 'K'
#define N_LINE_START '0'
#define N_LINE_END '$'

typedef enum {
  MODE_NORMAL,
  MODE_INSERT
} Mode;
const char *ModeNames[] = {"Normal", "Insert", "Visual", "Visual_line"};

Mode mode = MODE_NORMAL;

void normalMode(int input) {

  switch (input) {
  case N_ENTER_INSERT:
    mode = MODE_INSERT;
    renderSetCursorStyle(CURSOR_STYLE_BAR);
    break;
  case N_NEWLINE:
    nNewline();
    break;
  case N_LINE_END:
    nLineEnd();
    break;
  case N_LINE_START:
    nLineStart();
    break;
  case N_CURSOR_LEFT:
    nCursorLeft();
    break;
  case N_CURSOR_RIGHT:
    nCursorRight();
    break;
  case N_CURSOR_UP:
    nCursorUp();
    break;
  case N_CURSOR_DOWN:
    nCursorDown();
    break;
  }
}

void insertMode(int input) {
  switch (input) {
  case I_LEAVE_INSERT:
    mode = MODE_NORMAL;
    renderSetCursorStyle(CURSOR_STYLE_BLOCK);
    return;
  case I_BACKSPACE:
    iBackspace();
    break;
  default:
    bufferInsertChar(cursor.y, cursor.x, input);
    curInsertMoveRelative(1, 0);
    break;
  }
}

int main() {
  RenderContext render_ctx = {0};
  if (!renderInit(&render_ctx)) {
    fprintf(stderr, "failed to initialize renderer\n");
    return EXIT_FAILURE;
  }

  Buffer *buf = bufferNew();
  for (size_t i = 0; i < 5; i++) {
    bufferInsertLine(i, "chuj");
    /* code */
  }

  renderDraw(&render_ctx, cursor, ModeNames[mode]);

  while (true) {
    int input = renderGetInput(&render_ctx);

    if (mode == MODE_NORMAL && input == N_QUIT) {
      break;
    }

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

    renderDraw(&render_ctx, cursor, ModeNames[mode]);
  }

  renderShutdown(&render_ctx);
  return 0;
}
