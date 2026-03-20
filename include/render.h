#ifndef RENDER_H
#define RENDER_H

#include "cursor.h"
#include <stdbool.h>

typedef struct {
  void *main_window;
  void *numbers_window;
  void *status_window;
} RenderContext;

typedef enum {
  CURSOR_STYLE_BLOCK,
  CURSOR_STYLE_BAR,
  CURSOR_STYLE_UNDERSCORE,
} CursorStyle;

bool renderInit(RenderContext *ctx);
void renderShutdown(RenderContext *ctx);
void renderSetCursorStyle(CursorStyle style);
void renderDraw(RenderContext *ctx, Cursor cursor, const char *mode_name);
int renderGetInput(RenderContext *ctx);

#endif
