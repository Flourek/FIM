#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>

typedef struct {
  int row, col;
} Pos;

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

void bufferNewLine(Pos pos);
void bufferRemoveNewline(Pos pos);
void bufferMergeLine(int destRow, int srcRow, int colBreakpoint);
void bufferInsertChar(Pos pos, char ch);
void bufferDeleteChar(Pos pos);
void bufferReplaceChar(Pos pos, const char ch);
bool bufferIsCharBlank(Pos pos);
Word bufferGetNextWord(Pos pos);
Pos bufferTraverseFrom(Pos start, TraverseFn fn, bool reverse);
bool bufferIsCharGraph(Pos pos);
bool bufferIsCharBlank(Pos pos);
#endif
