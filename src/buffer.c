#include "buffer.h"
#include "helpers.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define LINE_CAPACITY 1024

static Buffer *buffers[1024];
static int buffer_count = 0;

static bool isValidPos(Pos pos) {
  Buffer *buf = bufferGet();

  if (pos.col < 0 || pos.col >= LINE_CAPACITY - 1)
    return false;

  if (pos.row < 0 || pos.row >= buf->line_count)
    return false;

  return true;
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

char *bufferGetLine(Pos pos) {
  Buffer *buf = bufferGet();

  if (!buf)
    return NULL;

  if (pos.row < 0 || pos.row >= buf->line_count)
    return NULL;

  return buf->lines[pos.row];
}

char bufferGetChar(Pos pos) {
  if (!isValidPos(pos))
    return '\0';

  char *line = bufferGetLine(pos);
  if (!line)
    return '\0';

  return line[pos.col];
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

void bufferReplaceChar(Pos pos, const char ch) {
  if (!isValidPos(pos))
    return;

  char *line = bufferGetLine(pos);
  if (!line)
    return;

  line[pos.col] = ch;
}

void bufferRemoveNewline(Pos pos) {
  if (!isValidPos(pos))
    return;

  char *line_above = bufferGetLine((Pos){pos.row - 1, 0});
  char *line = bufferGetLine(pos);
  if (!line_above || !line)
    return;

  memcpy(line + strlen(line), line_above, strlen(line_above));
  bufferDeleteLine(pos.row + 1);
}

void bufferMergeLine(int destRow, int srcRow, int colBreakpoint) {
  if (!isValidPos((Pos){srcRow, colBreakpoint}))
    return;

  if (!isValidPos((Pos){destRow, colBreakpoint}))
    return;

  char *src = bufferGetLine((Pos){srcRow, 0});
  char *dest = bufferGetLine((Pos){destRow, 0});
  if (!src || !dest)
    return;

  memcpy(dest + strlen(dest), src, strlen(src));
  bufferDeleteLine(srcRow);
}

void bufferNewLine(Pos pos) {
  if (!isValidPos(pos))
    return;

  char *line = bufferGetLine(pos);
  if (!line)
    return;

  bufferInsertLine(pos.row + 1, line + pos.col);
  line[pos.col] = '\0';
}

// --- line ops ---
void bufferInsertLine(int row, const char *text) {
  if (row < 0)
    return;

  Buffer *buf = bufferGet();

  if (row > buf->line_count)
    row = buf->line_count;

  buf->lines = realloc(buf->lines, sizeof(char *) * (buf->line_count + 1));

  // pushes rest of lines the new line, slow
  for (int i = buf->line_count; i > row; i--) {
    buf->lines[i] = buf->lines[i - 1];
  }

  buf->lines[row] = calloc(LINE_CAPACITY, sizeof(char));
  if (!buf->lines[row])
    return;

  if (text) {
    char *dup = strdupSafe(text);
    if (dup) {
      strncpy(buf->lines[row], dup, LINE_CAPACITY - 1);
      buf->lines[row][LINE_CAPACITY - 1] = '\0';
      free(dup);
    }
  }
  buf->line_count++;
}

void bufferDeleteLine(int index) {

  Buffer *buf = bufferGet();
  if (index < 0 || index >= buf->line_count)
    return;

  free(buf->lines[index]);
  memmove(&buf->lines[index], &buf->lines[index + 1], sizeof(char *) * (buf->line_count - index - 1));

  buf->line_count--;
  if (buf->line_count == 0) {
    free(buf->lines);
    buf->lines = NULL;
    return;
  }

  buf->lines = realloc(buf->lines, sizeof(char *) * buf->line_count);
}

int bufferLineLength(int row) {
  char *line = bufferGetLine((Pos){row, 0});
  return line ? (int)strlen(line) : 0;
}

void bufferInsertChar(Pos pos, char ch) {
  if (!isValidPos(pos))
    return;

  char *line = bufferGetLine(pos);
  if (!line)
    return;

  size_t len = strlen(line);

  if ((size_t)pos.col > len)
    pos.col = (int)len;

  if (len >= LINE_CAPACITY - 1)
    return;

  memmove(line + pos.col + 1, line + pos.col, len - (size_t)pos.col + 1);
  line[pos.col] = ch;
}

bool bufferDeleteChar(Pos pos) {
  if (!isValidPos(pos))
    return true;

  char *line = bufferGetLine(pos);
  if (!line)
    return true;

  size_t len = strlen(line);

  if ((size_t)pos.col >= len)
    return true;

  memmove(line + pos.col, line + pos.col + 1, len - (size_t)pos.col);
  return false;
}

bool bufferDeleteRange(Range range) {
  Pos start = range.start;
  Pos end = range.end;

  char *line = bufferGetLine(start);
  if (!line)
    return true;

  size_t len = strlen(line);
  if (start.col < 0 || (size_t)start.col >= len)
    return true;

  if (end.col < start.col)
    end.col = start.col;
  if ((size_t)end.col >= len)
    end.col = (int)len - 1;

  memmove(line + start.col, line + end.col + 1, len - (size_t)end.col);
  return false;
}

Pos bufferStart(void) {
  return (Pos){0, -1};
}

Pos bufferEnd(void) {
  Buffer *buf = bufferGet();
  if (!buf || buf->line_count <= 0)
    return bufferStart();

  int row = buf->line_count - 1;
  return (Pos){row, bufferLineLength(row)};
}

Pos bufferTraverse(Pos start, Pos end, TraverseFn fn) {
  bool forward = start.row < end.row || (start.row == end.row && start.col <= end.col);

  if (forward) {
    int xStart = start.col;

    for (int y = start.row; y <= end.row; y++) {
      char *line = bufferGetLine((Pos){y, 0});
      int xEnd = (y == end.row) ? end.col : (int)strlen(line);

      // if on the end row, iterate untill the final column, else full line
      for (int x = xStart; x <= xEnd; x++) {
        Pos pos = {y, x};
        char c = bufferGetChar(pos);

        bool result = fn(pos);
        if (result)
          return pos;
      }

      xStart = 0;
    }
  } else {
    int xStart = start.col;

    for (int y = start.row; y >= end.row; y--) {
      if (y != start.row)
        xStart = bufferLineLength(y);
      int xEnd = (y == end.row) ? end.col : 0;

      for (int x = xStart; x >= xEnd; x--) {
        Pos pos = {y, x};

        bool result = fn(pos);
        if (result)
          return pos;
      }
    }
  }

  return start;
}

bool bufferIsCharGraph(Pos pos) {
  char c = bufferGetChar(pos);
  return isgraph((unsigned char)c);
}

bool bufferIsCharBlank(Pos pos) {
  char c = bufferGetChar(pos);

  // file begin and end of lines
  if (c == '\0')
    return true;

  return isspace((unsigned char)c);
}

// WORD surrounded by newspace
Word bufferGetNextWord(Pos pos) {
  Pos blankPos = bufferTraverse(pos, bufferEnd(), bufferIsCharBlank);
  Pos newPos = bufferTraverse(blankPos, bufferEnd(), bufferIsCharGraph);

  // memccpy
  Word w = {0};
  return w;
}