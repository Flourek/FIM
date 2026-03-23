#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>

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
char *bufferGetLine(Pos pos);
char bufferGetChar(Pos pos);
void bufferInsertLine(int row, const char *text);
void bufferDeleteLine(int row);

int bufferLineLength(int row);
Pos bufferStart(void);
Pos bufferEnd(void);

void bufferNewLine(Pos pos);
void bufferRemoveNewline(Pos pos);
void bufferMergeLine(int destRow, int srcRow, int colBreakpoint);
void bufferInsertChar(Pos pos, char ch);
bool bufferDeleteChar(Pos pos);
bool bufferDeleteRange(Range range);
Range bufferNormalizeRange(Range range);
void bufferReplaceChar(Pos pos, const char ch);
bool bufferIsCharBlank(Pos pos);
Word bufferGetNextWord(Pos pos);
Pos bufferTraverse(Pos start, Pos end, TraverseFn fn);
bool bufferIsCharGraph(Pos pos);
bool bufferIsCharBlank(Pos pos);
#endif
