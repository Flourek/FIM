#include "render.h"
#include "command.h"
#include "helpers.h"
#include "search.h"
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
#define COLOR_BACKGROUND 10
#define COLOR_TEXT 11
#define COLOR_NUMBERS 12
#define init_color_rgb(id, r, g, b) init_color(id, r * 4, g * 4, b * 4)

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
  use_default_colors(); // allow -1 = terminal default background

  init_color_rgb(COLOR_BACKGROUND, 8, 33, 27);   // bright red
  init_color_rgb(COLOR_TEXT, 220, 220, 220);     // bright red
  init_color_rgb(COLOR_NUMBERS, 143, 143, 143);  // bright red
  init_pair(1, COLOR_BLACK, COLOR_TEXT);         // status bar
  init_pair(2, COLOR_TEXT, COLOR_BACKGROUND);    // normal transparent text/background
  init_pair(3, COLOR_NUMBERS, COLOR_BACKGROUND); // numbers
  set_escdelay(0);

  WINDOW *main_win = newwin(LINES - 2, COLS, 0, 0);
  if (!main_win) {
    endwin();
    return false;
  }

  keypad(main_win, TRUE);

  WINDOW *status_win = newwin(2, 0, LINES - 2, 0);
  // leaveok(status_win, TRUE);

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

char *makeStatusText(WINDOW *status_win) {
  int w = getmaxx(status_win);
  char *msg = malloc(w);
  memset(msg, ' ', w);

  char right[100];
  int rightLen = snprintf(right, 100, "%d,%d", cursor.row + 1, cursor.col + 1);
  // int rightLen = strlen(right);
  memcpy(msg + w - rightLen - 15, right, rightLen);

  Buffer *buf = bufferGet();
  char left[100];
  int leftLen = snprintf(left, 100, "DEBUG: mode=%s | LINES: %d | KEY: %s %d | MSG: %s", ModeNames[state.mode], buf->line_count, "chuj", state.last_key, state.log_message);
  memcpy(msg, left, leftLen);

  msg[w] = '\0';
  return msg;
}

void renderDrawStatus(RenderContext *ctx) {
  WINDOW *status_win = (WINDOW *)ctx->status_window;

  werase(status_win);

  Buffer *buf = bufferGet();

  wattron(status_win, A_BOLD);
  wattron(status_win, COLOR_PAIR(1));

  char *text = makeStatusText(status_win);
  mvwprintw(status_win, 0, 0, text);
  free(text);
  wattroff(status_win, COLOR_PAIR(1));

  if (state.mode == MODE_COMMAND) {
    const char *command = commandGetLineBuffer();
    mvwaddch(status_win, 1, 0, ':');
    mvwprintw(status_win, 1, 1, "%s", command);
  } else if (state.mode == MODE_SEARCH) {
    const char *query = searchGetLineBuffer();
    mvwaddch(status_win, 1, 0, '/');
    mvwprintw(status_win, 1, 1, "%s", query);
  }

  // wclrtoeol(status_win);
  wattroff(status_win, A_BOLD);

  // log("");
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

  wmove(main_win, cursor.row - viewportStart.row, 4 + cursor.col - viewportStart.col);
  wrefresh(main_win);
}

void renderSetCursor(RenderContext *ctx) {
  WINDOW *status_win = (WINDOW *)ctx->status_window;
  WINDOW *main_win = (WINDOW *)ctx->main_window;

  // log("%i %i %i", chuj, classAt(cursor), isLineEmpty(cursor.row));
  if (state.mode == MODE_COMMAND) {
    // wmove(status_win, 1, 0);
    wrefresh(status_win);
    // waddch(status_win, "E");
  } else {
    wrefresh(main_win);
    // wmove(main_win, cursor.row - viewportStart.row, 4 + cursor.col - viewportStart.col);
  }
}

void renderDraw(RenderContext *ctx, Cursor cursor) {
  renderDrawBuffer(ctx);
  renderDrawStatus(ctx);
  renderSetCursor(ctx);
}

int renderGetInput(RenderContext *ctx, wint_t *ch) {
  WINDOW *main_win = (WINDOW *)ctx->main_window;
  int ret = wget_wch(main_win, ch);

  // log("keyType=%d ch_dec=%d ch_hex=%04X", ret, (int)*ch, (int)*ch);

  return ret;
}
