#include "buffer.h"
#include "cursor.h"
#include "input.h"
#include "render.h"
#include "state.h"
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  RenderContext render_ctx = {0};
  if (!renderInit(&render_ctx)) {
    fprintf(stderr, "failed to initialize renderer\n");
    return EXIT_FAILURE;
  }

  Buffer *buf = bufferNew();
  bufferInsertLine(0, "loremipsumdolorsit amet");
  bufferInsertLine(1, "    consectetur adipiscing elit");
  bufferInsertLine(2, "");
  bufferInsertLine(3, "");
  bufferInsertLine(4, "t");
  bufferInsertLine(5, "ut enim ad minim veniam");

  renderDraw(&render_ctx, cursor, ModeNames[state.mode]);

  while (true) {
    int input = renderGetInput(&render_ctx);

    if (inputHandle(input)) {
      break;
    }

    renderDraw(&render_ctx, cursor, ModeNames[state.mode]);
  }

  renderShutdown(&render_ctx);
  return 0;
}
