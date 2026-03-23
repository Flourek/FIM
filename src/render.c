#include "render.h"
#include "helpers.h"
#include "state.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "utf8.h"
#include <wchar.h>
#define ANSI_CURSOR_BLOCK "\x1b[2 q"
#define ANSI_CURSOR_BAR "\x1b[6 q"
#define ANSI_CURSOR_UNDERSCORE "\x1b[4 q"

#define RELATIVE_NUMBERS 1

static Pos viewportStart = {10, 0};

static const char *
humanKeyName(int key) {
  const char *name = keyname(key);
  return name ? name : "?";
}

static void formatLineNumber(int row, int line_count, char out[]) {
  (void)line_count;

  int number = row + 1;
  int relative = abs(row - cursor.row);

  if (RELATIVE_NUMBERS && relative != 0) {
    number = relative;
  }

  snprintf(out, sizeof(out), "%d", number);
}

static void disable_flow_control(void) {
  struct termios t;
  if (tcgetattr(STDIN_FILENO, &t) == 0) {
    t.c_iflag &= ~(IXON | IXOFF);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
  }
}

bool renderInit(RenderContext *ctx) {
  initscr();
  cbreak();
  noecho();

  state.ctx = ctx;
  // in Render_init(), once (not in draw)
  start_color();
  disable_flow_control();
  use_default_colors();                   // allow -1 = terminal default background
  init_pair(1, COLOR_BLACK, COLOR_WHITE); // status bar
  init_pair(2, -1, -1);                   // normal transparent text/background
  init_pair(3, 8, -1);                    // normal transparent text/background
  set_escdelay(0);

  WINDOW *main_win = newwin(LINES - 2, COLS, 0, 0);
  if (!main_win) {
    endwin();
    return false;
  }

  keypad(main_win, TRUE);

  WINDOW *status_win = newwin(2, 0, LINES - 2, 0);
  leaveok(status_win, TRUE);

  ctx->main_window = main_win;
  ctx->status_window = status_win;

  wbkgd((WINDOW *)ctx->main_window, COLOR_PAIR(2));
  wbkgd((WINDOW *)ctx->status_window, COLOR_PAIR(2));

  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  return true;
}

void renderShutdown(RenderContext *ctx) {
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);

  WINDOW *main_win = (WINDOW *)ctx->main_window;
  if (main_win) {
    delwin(main_win);
    ctx->main_window = NULL;
  }
  endwin();
}

void renderSetCursorStyle(CursorStyle style) {
  const char *seq;
  switch (style) {
  case CURSOR_STYLE_BAR:
    seq = ANSI_CURSOR_BAR;
    break;
  case CURSOR_STYLE_UNDERSCORE:
    seq = ANSI_CURSOR_UNDERSCORE;
    break;
  default:
    seq = ANSI_CURSOR_BLOCK;
    break;
  }

  if (isatty(STDOUT_FILENO)) {
    fputs(seq, stdout);
    fflush(stdout);
  }
}

void renderDrawStatus(RenderContext *ctx, const char *mode_name) {
  WINDOW *status_win = (WINDOW *)ctx->status_window;

  werase(status_win);

  Buffer *buf = bufferGet();

  int w = getmaxx(status_win);
  char msg[100] = "";
  snprintf(msg, 100, "%d,%d    ", cursor.row + 1, cursor.col + 1);
  int msg_len = (int)strlen(msg);
  int x = w - msg_len;
  if (x < 0)
    x = 0;
  wattron(status_win, COLOR_PAIR(1));
  mvwhline(status_win, 0, 0, ' ', x);
  mvwprintw(status_win, 0, x, "%s", msg);
  wattroff(status_win, COLOR_PAIR(1));

  wattron(status_win, A_BOLD);

  mvwprintw(status_win, 1, 0, "DEBUG: mode=%s | LINES: %d | KEY: %s %d | MSG: %s", mode_name, buf->line_count, humanKeyName(state.last_key), state.last_key, state.log_message);
  wclrtoeol(status_win);
  wattroff(status_win, A_BOLD);

  log("");
  wrefresh(status_win);
}

Pos renderViewportOffset(RenderContext *ctx) {

  WINDOW *main_win = (WINDOW *)ctx->main_window;
  int h = getmaxy(main_win);
  Pos out = {0, 0};

  // if (cursor.row + 1 >= viewportStart.row + h) {
  //   viewportStart = (Pos){cursor.row, 0};
  // }

  // screencoordinate = cursor.row % h
  // viewportStart.row +
  out.row = cursor.row % h;

  // log("height: %i cursor: %i out: %i viewport: %i", h, cursor.row, out.row, viewportStart.row);
  return out;
}

void renderDrawBuffer(RenderContext *ctx) {
  WINDOW *main_win = (WINDOW *)ctx->main_window;

  werase(main_win);

  Buffer *buf = bufferGet();

  char msg[100] = "";

  Pos scrollOffset = {0, 0};
  int h = getmaxy(main_win);
  int w = getmaxx(main_win);

  int numbersWidth = 4;
  int viewportWidth = (w - 1) - numbersWidth;

  // Convert cursor to visual columns for viewport calculation
  char *cursorLine = bufferGetLine(cursor.row);
  int cursorVisual = utf8_byte_to_column(cursorLine, cursor.col);

  if (cursorVisual > viewportStart.col + viewportWidth) {
    viewportStart.col = cursorVisual - viewportWidth;
  }

  if (cursorVisual < viewportStart.col) {
    viewportStart.col = cursorVisual;
  }

  if (cursor.row > viewportStart.row + (h - 1)) {
    viewportStart.row = cursor.row - (h - 1);
  }

  if (cursor.row < viewportStart.row) {
    viewportStart.row = cursor.row;
  }

  if (viewportStart.row < 0)
    viewportStart.row = 0;

  if (viewportStart.col < 0)
    viewportStart.col = 0;

  // for every viewport line
  for (int i = 0; i < h; i++) {
    int line_index = i + viewportStart.row;

    if (line_index < buf->line_count) {

      // PRINTING LINE NUMBERS
      char number_str[64];
      formatLineNumber(line_index, buf->line_count, number_str);

      int number_len = (int)strlen(number_str);

      wattron(main_win, COLOR_PAIR(3));
      if (line_index == cursor.row) {
        mvwprintw(main_win, i, 0, number_str);
      } else {
        mvwprintw(main_win, i, numbersWidth - number_len - 1, number_str);
      }
      wattroff(main_win, COLOR_PAIR(3));

      // PRINTING LINE CONTENT
      const char *line = bufferGetLine(line_index);
      if (line) {
        int byteOffset = utf8_column_to_byte(line, viewportStart.col);
        wmove(main_win, i, numbersWidth);
        waddnstr(main_win, line + byteOffset, viewportWidth + 1);
      }
    } else {
      // UNUSED LINE
      wattron(main_win, COLOR_PAIR(3));
      mvwprintw(main_win, i, 0, "~");
      wattroff(main_win, COLOR_PAIR(3));
    }
  }

  wrefresh(main_win);
  renderSetCursor(main_win, numbersWidth);
}

void renderSetCursor(WINDOW *win, int numbersWidth) {
  char *test = bufferGetLine(cursor.row);
  int idx = utf8_column_to_byte(test, cursor.col);
  log("%i c: ", utf8_next(test, idx));
  wmove(win, cursor.row - viewportStart.row, numbersWidth + cursor.col - viewportStart.col);
}

void renderDraw(RenderContext *ctx, Cursor cursor, const char *mode_name) {
  renderDrawBuffer(ctx);
  renderDrawStatus(ctx, mode_name);
}

int renderGetInput(RenderContext *ctx, wint_t *ch) {
  WINDOW *main_win = (WINDOW *)ctx->main_window;
  int ret = wget_wch(main_win, ch);
  state.last_key = (int)*ch;

  // log("keyType=%d ch_dec=%d ch_hex=%04X", ret, (int)*ch, (int)*ch);

  return ret;
}
