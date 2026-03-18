#include "render.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ANSI_CURSOR_BLOCK "\x1b[2 q"
#define ANSI_CURSOR_BAR "\x1b[6 q"

#define RELATIVE_NUMBERS 1

int last_key = 'E';

static const char *humanKeyName(int key) {
  const char *name = keyname(key);
  return name ? name : "?";
}

static void formatLineNumber(int row, int line_count, char out[]) {
  (void)line_count;

  int number = row;
  int relative = abs(row - cursor.y);

  if (RELATIVE_NUMBERS && relative != 0) {
    number = relative;
  }

  snprintf(out, sizeof(out), "%d", number);
}

bool renderInit(RenderContext *ctx) {
  initscr();
  cbreak();
  noecho();

  // in Render_init(), once (not in draw)
  start_color();
  use_default_colors();                   // allow -1 = terminal default background
  init_pair(1, COLOR_BLACK, COLOR_WHITE); // status bar
  init_pair(2, -1, -1);                   // normal transparent text/background
  init_pair(3, 8, -1);                    // normal transparent text/background
  set_escdelay(0);

  WINDOW *main_win = newwin(LINES - 2, COLS - 4, 0, 4);
  if (!main_win) {
    endwin();
    return false;
  }

  keypad(main_win, TRUE);

  WINDOW *numbers_win = newwin(LINES - 2, 4, 0, 0);
  leaveok(numbers_win, TRUE);

  WINDOW *status_win = newwin(2, 0, LINES - 2, 0);
  leaveok(status_win, TRUE);

  ctx->main_window = main_win;
  ctx->numbers_window = numbers_win;
  ctx->status_window = status_win;

  wbkgd((WINDOW *)ctx->main_window, COLOR_PAIR(2));
  wbkgd((WINDOW *)ctx->numbers_window, COLOR_PAIR(3));
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
  const char *seq = (style == CURSOR_STYLE_BAR) ? "\x1b[6 q" : "\x1b[2 q";
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
  snprintf(msg, 100, "%d,%d    ", cursor.x + 1, cursor.y);
  int msg_len = (int)strlen(msg);
  int x = w - msg_len;
  if (x < 0)
    x = 0;
  wattron(status_win, COLOR_PAIR(1));
  mvwhline(status_win, 0, 0, ' ', x);
  mvwprintw(status_win, 0, x, "%s", msg);
  wattroff(status_win, COLOR_PAIR(1));

  wattron(status_win, A_BOLD);
  mvwprintw(status_win, 1, 0, "DEBUG: mode=%s | LINES: %d | KEY: %s %d", mode_name, buf->line_count, humanKeyName(last_key), last_key);
  wclrtoeol(status_win);
  wattroff(status_win, A_BOLD);

  wrefresh(status_win);
}

void renderDrawBuffer(RenderContext *ctx) {
  WINDOW *main_win = (WINDOW *)ctx->main_window;

  werase(main_win);

  Buffer *buf = bufferGet();

  for (int i = 0; i < buf->line_count; i++) {
    mvwprintw(main_win, i, 0, "%s", buf->lines[i]);
  }

  wmove(main_win, cursor.y, cursor.x);
  wrefresh(main_win);
}

void renderDrawNumbers(RenderContext *ctx) {
  WINDOW *number_win = (WINDOW *)ctx->numbers_window;

  werase(number_win);

  Buffer *buf = bufferGet();

  wattron(number_win, COLOR_PAIR(3));
  for (int i = 0; i < LINES; i++) {
    if (i < buf->line_count) {
      char number_str[64];
      formatLineNumber(i, buf->line_count, number_str);

      int number_len = (int)strlen(number_str);
      int w = getmaxx(number_win);

      if (i == cursor.y) {
        mvwprintw(number_win, i, 0, number_str);
      } else {
        mvwprintw(number_win, i, w - number_len - 1, number_str);
      }
    } else {
      mvwprintw(number_win, i, 0, "~");
    }
  }
  wattroff(number_win, COLOR_PAIR(3));
  wrefresh(number_win);
}

void renderDraw(RenderContext *ctx, Cursor cursor, const char *mode_name) {
  renderDrawNumbers(ctx);
  renderDrawStatus(ctx, mode_name);
  renderDrawBuffer(ctx);
}

int renderGetInput(RenderContext *ctx) {
  WINDOW *main_win = (WINDOW *)ctx->main_window;
  last_key = wgetch(main_win);
  return last_key;
}
