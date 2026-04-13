#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <wchar.h>

typedef struct {
  int row, col;
} Pos;

typedef struct {
  Pos start;
  Pos end;
  bool inclusive;
  bool valid;
} Range;

#define INVALID_RANGE            \
  (Range) {                      \
    {0, 0}, {0, 0}, false, false \
  }

typedef struct {
  int index;
  char **lines;
  int line_count;
  char *path;

} Buffer;

typedef bool (*TraverseFn)(Pos pos);

Buffer *bufferNew(const char *path);
void bufferFree(Buffer *buf);
Buffer *bufferGet();

char *bufferGetLine(int row);
wchar_t bufferGetWChar(Pos pos);

void bufferInsertLine(int row, const char *text);
void bufferDeleteLine(int row);

int bufferLineLength(int row);
Pos bufferStart(void);
Pos bufferEnd(void);

void bufferNewLine(Pos pos);
void bufferMergeLine(int destRow, int srcRow, int colBreakpoint);

void bufferInsertChar(Pos pos, wint_t ch);
bool bufferDeleteChar(Pos pos);
void bufferReplaceChar(Pos pos, const int ch);

// Position-based wrappers used with bufferTraverse.
bool bufferIsCharBlank(Pos pos);
bool bufferIsCharGraph(Pos pos);

Pos bufferPrevWChar(Pos pos);
Pos bufferNextWChar(Pos pos);
bool isBufferStart(Pos pos);
bool isBufferEnd(Pos pos);
bool isLineEmpty(int row);

int comparePos(Pos a, Pos b);
bool bufferDeleteRange(Range range);
Range bufferNormalizeRange(Range range);
void bufferClear(void);
const char *bufferGetFilename(void);
#endif
