#include "buffer.h"
#include "helpers.h"
#include "utf8.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>

#define LINE_CAPACITY 1024

static Buffer *buffers[1024];
static int buffer_count = 0;

// all pos args are screen cursor grid aligned, utf8 only internal ahh b should...

static bool isValidPos(Pos pos) {
  Buffer *buf = bufferGet();

  if (pos.col < 0 || pos.col >= LINE_CAPACITY - 1)
    return false;

  if (pos.row < 0 || pos.row >= buf->line_count)
    return false;

  return true;
}

int comparePos(Pos a, Pos b) {
  if (a.row != b.row)
    return a.row - b.row;
  return a.col - b.col;
}

Buffer *bufferNew(const char *path) {
  Buffer *buf = malloc(sizeof(Buffer));
  buf->lines = NULL;
  buf->line_count = 0;
  buf->index = buffer_count;
  buf->path = NULL;
  if (path) {
    buf->path = strdupSafe(path);
  }
  buffers[buffer_count] = buf;
  buffer_count++;
  return buf;
}

Buffer *bufferGet() {
  return buffers[0];
}

char *bufferGetLine(int row) {
  Buffer *buf = bufferGet();

  if (!buf)
    return NULL;

  if (row < 0 || row >= buf->line_count)
    return NULL;

  return buf->lines[row];
}

wchar_t bufferGetWChar(Pos pos) {
  if (!isValidPos(pos))
    return L'\0';

  char *line = bufferGetLine(pos.row);
  if (!line)
    return L'\0';

  size_t len = bufferLineLength(pos.row);
  int idx = utf8_column_to_byte(line, pos.col);

  wchar_t wc;
  int res = mbtowc(&wc, line + idx, len - (size_t)idx);
  if (res <= 0)
    return L'\0';

  return wc;
}

void bufferFree(Buffer *buf) {
  if (!buf)
    return;

  for (int i = 0; i < buf->line_count; i++) {
    free(buf->lines[i]);
  }
  free(buf->lines);
  if (buf->path) {
    free(buf->path);
  }
  free(buf);
  buffer_count--;
}

void bufferReplaceChar(Pos pos, const int ch) {
  if (!isValidPos(pos))
    return;

  char *line = bufferGetLine(pos.row);
  if (!line)
    return;

  line[pos.col] = ch;
}

void bufferMergeLine(int destRow, int srcRow, int colBreakpoint) {
  if (!isValidPos((Pos){srcRow, colBreakpoint}))
    return;

  if (!isValidPos((Pos){destRow, colBreakpoint}))
    return;

  char *src = bufferGetLine(srcRow);
  char *dest = bufferGetLine(destRow);
  if (!src || !dest)
    return;

  memcpy(dest + strlen(dest), src, strlen(src));
  bufferDeleteLine(srcRow);
}

void bufferNewLine(Pos pos) {
  if (!isValidPos(pos))
    return;

  char *line = bufferGetLine(pos.row);
  if (!line)
    return;

  int byteCol = utf8_column_to_byte(line, pos.col);
  bufferInsertLine(pos.row + 1, line + byteCol);
  line[byteCol] = '\0';
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
  char *line = bufferGetLine(row);
  return line ? (int)strlen(line) : 0;
}

void bufferInsertChar(Pos pos, wint_t ch) {
  if (!isValidPos(pos))
    return;

  char *line = bufferGetLine(pos.row);
  if (!line)
    return;

  size_t len = strlen(line);

  if ((size_t)pos.col > len)
    pos.col = (int)len;

  if (len >= LINE_CAPACITY - 1)
    return;

  char buf[8];
  size_t charLen = utf8_encode(buf, ch);
  size_t byteCol = (size_t)utf8_column_to_byte(line, pos.col);

  memmove(line + byteCol + charLen, line + byteCol, len - (size_t)byteCol + charLen);
  memcpy(line + byteCol, buf, charLen);
}

bool bufferDeleteChar(Pos pos) {
  if (!isValidPos(pos))
    return true;

  char *line = bufferGetLine(pos.row);
  size_t len = strlen(line);

  if ((size_t)pos.col >= len)
    return true;

  size_t byteCol = (size_t)utf8_column_to_byte(line, pos.col);
  size_t charLen = utf8_char_len(line, byteCol);
  size_t n = len - (size_t)byteCol - charLen + 1;

  memmove(line + byteCol, line + byteCol + charLen, n);
  return false;
}

bool bufferDeleteRange(Range range) {
  if (!range.valid)
    return false;

  Buffer *buf = bufferGet();
  if (!buf || buf->line_count <= 0)
    return true;

  Pos start = range.start;
  Pos end = range.end;

  if (start.row < 0 || start.row >= buf->line_count)
    return true;
  if (end.row < 0 || end.row >= buf->line_count)
    return true;

  char *startLine = bufferGetLine(start.row);
  char *endLine = bufferGetLine(end.row);
  if (!startLine || !endLine)
    return true;

  size_t startLen = strlen(startLine);
  size_t endLen = strlen(endLine);

  // Clamp both columns to valid ranges on their current lines.
  if (start.col < 0)
    start.col = 0;
  if ((size_t)start.col > startLen)
    start.col = (int)startLen;

  if (end.col < 0)
    end.col = -1;
  if ((size_t)end.col > endLen)
    end.col = (int)endLen;

  // Single-line delete: remove [start.col, end.col] in place.
  if (start.row == end.row) {
    if (startLen == 0 || (size_t)start.col >= startLen)
      return false;

    if (end.col < start.col)
      end.col = start.col;
    if ((size_t)end.col >= startLen)
      end.col = (int)startLen - 1;

    memmove(startLine + start.col, startLine + end.col + 1, startLen - (size_t)end.col);
    return false;
  }

  // Multi-line delete:
  // keep front of start line + keep tail of end line, then delete lines in between.
  size_t keepFrontLen = (size_t)start.col;
  size_t keepTailFrom = (size_t)(end.col + 1);
  if (keepTailFrom > endLen)
    keepTailFrom = endLen;
  size_t keepTailLen = endLen - keepTailFrom;

  size_t maxTailLen = (LINE_CAPACITY - 1) - keepFrontLen;
  if (keepTailLen > maxTailLen)
    keepTailLen = maxTailLen;

  memmove(startLine + keepFrontLen, endLine + keepTailFrom, keepTailLen);
  startLine[keepFrontLen + keepTailLen] = '\0';

  int linesToDelete = end.row - start.row;
  for (int i = 0; i < linesToDelete; i++)
    bufferDeleteLine(start.row + 1);

  return false;
}

Range bufferNormalizeRange(Range range) {
  if (!range.valid)
    return range;

  bool backward = comparePos(range.start, range.end) > 0;

  if (backward) {
    Pos tmp = range.start;
    range.start = range.end;
    range.end = tmp;
  }

  // w, b
  if (!range.inclusive) {
    range.end.col--;
  }

  // log("CHUJ: %i,%i - %i,%i, %s", range.start.row, range.start.col, range.end.row, range.end.col, range.inclusive ? "Inclusive" : "Exclusive");

  return range;
}

Pos bufferNextWChar(Pos pos) {
  Pos out;

  char *line = bufferGetLine(pos.row);
  int posIndex = utf8_column_to_byte(line, pos.col);
  int nextIndex = utf8_next(line, posIndex);
  out = (Pos){pos.row, utf8_byte_to_column(line, nextIndex)};

  if (posIndex >= bufferLineLength(pos.row)) {
    out.row = pos.row + 1;
    out.col = 0;
  }

  return out;
}

Pos bufferPrevWChar(Pos pos) {
  Pos out;

  char *line = bufferGetLine(pos.row);
  int posIndex = utf8_column_to_byte(line, pos.col);
  int prevIndex = utf8_prev(line, posIndex);
  out = (Pos){pos.row, utf8_byte_to_column(line, prevIndex)};

  if (posIndex <= 0) {
    out.row = pos.row - 1;
    out.col = bufferLineLength(out.row);

    if (out.row < 0) {
      return bufferStart();
    }
  }

  return out;
}

Pos bufferStart(void) {
  return (Pos){0, 0};
}

Pos bufferEnd(void) {
  Buffer *buf = bufferGet();
  if (!buf || buf->line_count <= 0)
    return bufferStart();

  int row = buf->line_count - 1;
  return (Pos){row, bufferLineLength(row)};
}

bool isBufferStart(Pos pos) {
  Pos start = bufferStart();
  return comparePos(pos, start) <= 0;
}

bool isBufferEnd(Pos pos) {
  Pos end = bufferEnd();
  return comparePos(pos, end) >= 0;
}

bool isLineEmpty(int row) {
  return bufferGetWChar((Pos){row, 0}) == '\0';
}

void bufferClear(void) {
  Buffer *buf = bufferGet();
  if (!buf || !buf->lines)
    return;

  for (int i = 0; i < buf->line_count; i++) {
    free(buf->lines[i]);
  }

  free(buf->lines);
  buf->lines = NULL;
  buf->line_count = 0;
}

const char *bufferGetFilename(void) {
  Buffer *buf = bufferGet();
  if (!buf || !buf->path)
    return NULL;

  const char *base = strrchr(buf->path, '/');
  return base ? base + 1 : buf->path;
}
