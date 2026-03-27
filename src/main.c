#include "buffer.h"
#include "cursor.h"
#include "input.h"
#include "render.h"
#include "state.h"
#include "utf8.h"
#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  setlocale(LC_ALL, "");

  RenderContext render_ctx = {0};
  if (!renderInit(&render_ctx)) {
    fprintf(stderr, "failed to initialize renderer\n");
    return EXIT_FAILURE;
  }

  Buffer *buf = bufferNew();
  bufferInsertLine(0, "loremipsumdolorsit aametloremipsumdolorsitametloremipsumdolo rsitametloremipsumdolorsitmetloremipsumdolorsitametoremips umdolorsitamet");
  bufferInsertLine(1, "consectetur adipiscing elit");
  bufferInsertLine(2, "");
  bufferInsertLine(3, "");
  bufferInsertLine(4, "tloremipsumdolorsit ametloremipsumdolorsit amet");
  bufferInsertLine(5, "t");
  bufferInsertLine(6, "ut enim ad minim veniamloremipsumdolorsit ametloremipsumdolorsit amet");
  bufferInsertLine(7, "consectetur adipiscing elit");
  bufferInsertLine(8, "");
  bufferInsertLine(9, "");
  bufferInsertLine(10, "t");
  bufferInsertLine(11, "ut enim ad minim veniam");
  bufferInsertLine(12, "ut enim ad minim veniam");
  bufferInsertLine(13, "consectetur adipiscing elit");
  bufferInsertLine(14, "");
  bufferInsertLine(15, "");
  bufferInsertLine(17, "t");
  bufferInsertLine(18, "ut enim ad minim veniam");
  bufferInsertLine(19, "ut enim ad minim veniam");
  bufferInsertLine(20, "consectetur adipiscing elit");
  bufferInsertLine(21, "t");
  bufferInsertLine(22, "ut enim ad minim veniam");
  bufferInsertLine(23, "ut enim ad minim veniam");
  bufferInsertLine(24, "consectetur adipiscing elit");
  bufferInsertLine(25, "");
  bufferInsertLine(26, "");
  bufferInsertLine(27, "t");
  bufferInsertLine(28, "ut enim ad minim veniam");
  bufferInsertLine(29, "ut enim ad minim veniam");
  bufferInsertLine(30, "consectetur adipiscing elit");
  renderDraw(&render_ctx, cursor);

  while (true) {
    wint_t ch;
    int keyType = renderGetInput(&render_ctx, &ch);

    if (inputHandle(keyType, ch)) {
      break;
    }

    renderDraw(&render_ctx, cursor);
  }

  renderShutdown(&render_ctx);
  return 0;
}
