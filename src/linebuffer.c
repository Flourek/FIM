#include "linebuffer.h"

void lineBufferClear(LineBuffer *lb) {
  if (!lb)
    return;
  lb->len = 0;
  lb->data[0] = '\0';
}

const char *lineBufferCStr(const LineBuffer *lb) {
  return lb ? lb->data : "";
}

void lineBufferInsertWChar(LineBuffer *lb, wchar_t wc) {
  if (!lb)
    return;

  char tmp[5];
  int n = utf8_encode(tmp, (wint_t)wc);
  if (n <= 0)
    return;

  if (lb->len + n >= (int)sizeof(lb->data))
    return; // no space

  memcpy(lb->data + lb->len, tmp, n);
  lb->len += n;
  lb->data[lb->len] = '\0';
}

void lineBufferBackspace(LineBuffer *lb) {
  if (!lb)
    return;

  if (lb->len == 0)
    return;

  int newLen = utf8_prev(lb->data, lb->len);
  lb->len = newLen;
  lb->data[lb->len] = '\0';
}
