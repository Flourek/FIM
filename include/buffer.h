#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
  int index;
  char **lines;
  int line_count;
} Buffer;

Buffer *bufferNew(void);
void bufferFree(Buffer *buf);
Buffer *bufferGet();
void bufferInsertLine(int line, const char *text);
void bufferNewline();
void bufferDeleteLine(Buffer *buf, int index);
int bufferLineLength(int row);

void bufferInsertChar(int row, int col, char ch);
void bufferDeleteChar(int row, int col);

#endif
