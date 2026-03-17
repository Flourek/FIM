#include "buffer.h"
#include <stdlib.h>
#include <string.h>

static Buffer *buffers[1024];
static int buffer_count = 0;

static char *strdup_safe(const char *s) {
  if (!s)
    return NULL;
  size_t len = strlen(s);
  char *out = malloc(len + 1);
  memcpy(out, s, len + 1);
  return out;
}

Buffer *Buffer_new(void) {
  Buffer *buf = malloc(sizeof(Buffer));
  buf->lines = NULL;
  buf->line_count = 0;
  buf->index = buffer_count;
  buffers[buffer_count] = buf;
  buffer_count++;
  return buf;
}

Buffer *Buffer_get() {
  return buffers[0];
}

void Buffer_free(Buffer *buf) {
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
void Buffer_insert_line(int index, const char *text) {
  if (index < 0)
    return;

  Buffer *buf = Buffer_get();

  if (index > buf->line_count) {
    buf->line_count = index;
  }

  buf->lines = realloc(buf->lines, sizeof(char *) * (buf->line_count + 1));

  // pushes rest of lines the new line, slow
  for (int i = buf->line_count; i > index; i--) {
    buf->lines[i] = buf->lines[i - 1];
  }

  buf->lines[index] = strdup_safe(text ? text : "");
  buf->line_count++;
}

void Buffer_insert_char(int line, int col, char ch) {

  Buffer *buf = Buffer_get();
  if (line > buf->line_count) {
    return;
  }
  buf->lines[line][col] = ch;
};