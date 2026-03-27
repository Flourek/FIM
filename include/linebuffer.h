#ifndef LINEBUFFER_H
#define LINEBUFFER_H

#include "utf8.h"

#include <wchar.h>

typedef struct {
  char data[1024];
  int len;
} LineBuffer;

void lineBufferClear(LineBuffer *lb);
const char *lineBufferCStr(const LineBuffer *lb);
void lineBufferInsertWChar(LineBuffer *lb, wchar_t wc);
void lineBufferBackspace(LineBuffer *lb);

#endif
