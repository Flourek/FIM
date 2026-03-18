#include "buffer.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static Buffer *buffers[1024];
static int buffer_count = 0;

static char *strdupSafe(const char *s) {
  if (!s)
    return NULL;
  size_t len = strlen(s);
  char *out = malloc(len + 1);
  memcpy(out, s, len + 1);
  return out;
}

Buffer *bufferNew(void) {
  Buffer *buf = malloc(sizeof(Buffer));
  buf->lines = NULL;
  buf->line_count = 0;
  buf->index = buffer_count;
  buffers[buffer_count] = buf;
  buffer_count++;
  return buf;
}

Buffer *bufferGet() {
  return buffers[0];
}

void bufferFree(Buffer *buf) {
  if (!buf)
    return;

  for (int i = 0; i < buf->line_count; i++) {
    free(buf->lines[i]);
  }
  free(buf->lines);
  free(buf);
  buffer_count--;
}

// --- line ops ---
void bufferInsertLine(int row, const char *text) {
  if (row < 0)
    return;

  Buffer *buf = bufferGet();

  if (row > buf->line_count) {
    buf->line_count = row;
  }

  buf->lines = realloc(buf->lines, sizeof(char *) * (buf->line_count + 1));

  // pushes rest of lines the new line, slow
  for (int i = buf->line_count; i > row; i--) {
    buf->lines[i] = buf->lines[i - 1];
  }

  char emptyline[1024] = {0};
  buf->lines[row] = strdupSafe(emptyline);
  buf->line_count++;
}

int bufferLineLength(int row) {
  return strlen(bufferGet()->lines[row]);
}

bool isValidRow(int row) {
  Buffer *buf = bufferGet();
  return row > buf->line_count;
}

void bufferInsertChar(int row, int col, char ch) {
  if (!isValidRow)
    return;

  Buffer *buf = bufferGet();

  buf->lines[row][col] = ch;
};

void bufferDeleteChar(int row, int col) {
  if (!isValidRow)
    return;

  Buffer *buf = bufferGet();
  int prevCharPos = row - 1;
  if (prevCharPos < 0)
    prevCharPos = 0;

  buf->lines[row][col] = " ";
}