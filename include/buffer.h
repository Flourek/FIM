#ifndef BUFFER_H
#define BUFFER_H

typedef struct {
  int index;
  char **lines;
  int line_count;
} Buffer;

Buffer *Buffer_new(void);
void Buffer_free(Buffer *buf);
Buffer *Buffer_get();
void Buffer_insert_line(int index, const char *text);
void Buffer_newline();
void Buffer_delete_line(Buffer *buf, int index);

void Buffer_insert_char(int line, int col, char ch);
void Buffer_delete_char(Buffer *buf, int line, int col);

#endif
