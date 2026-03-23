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
} Range;

typedef struct {
  int index;
  char **lines;
  int line_count;
} Buffer;

typedef struct
{
  int x, y;
  char *text;
  int len;
} Word;

typedef bool (*TraverseFn)(Pos pos);

Buffer *bufferNew(void);
void bufferFree(Buffer *buf);
Buffer *bufferGet();

char *bufferGetLine(int row);
char bufferGetChar(Pos pos);

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

Pos bufferTraverse(Pos start, Pos end, TraverseFn fn);

// Char-level classification helpers (for UTF-8 / wchar-aware logic).
bool bufferIsCharBlankChar(char c);
bool bufferIsCharGraphChar(char c);

// Position-based wrappers used with bufferTraverse.
bool bufferIsCharBlank(Pos pos);
bool bufferIsCharGraph(Pos pos);

bool bufferDeleteRange(Range range);
Range bufferNormalizeRange(Range range);
Word bufferGetNextWord(Pos pos);
#endif
