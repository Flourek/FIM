#include "buffer.h"
#include "command.h"
#include "cursor.h"
#include "files.h"
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

  bufferNew(NULL);
  filesLoadIntoBuffer("./save/DEFAULT", bufferGet());
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
