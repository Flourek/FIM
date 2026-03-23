#include "files.h"
#include <stdio.h>
#include <string.h>

#define FILE_LINE_CAPACITY 1024

bool filesLoadIntoBuffer(const char *path, Buffer *buf) {
  if (!path || !buf)
    return false;

  FILE *fp = fopen(path, "r");
  if (!fp)
    return false;

  // TODO: if you want load to replace content, delete old lines first.
  char line[FILE_LINE_CAPACITY];
  int row = 0;

  while (fgets(line, sizeof(line), fp)) {
    line[strcspn(line, "\r\n")] = '\0';
    bufferInsertLine(row, line);
    row++;
  }

  fclose(fp);
  return true;
}

bool filesSaveFromBuffer(const char *path, const Buffer *buf) {
  if (!path || !buf)
    return false;

  FILE *fp = fopen(path, "w");
  if (!fp)
    return false;

  for (int row = 0; row < buf->line_count; row++) {
    const char *line = bufferGetLine(row);
    if (fputs(line ? line : "", fp) == EOF) {
      fclose(fp);
      return false;
    }

    if (row + 1 < buf->line_count && fputc('\n', fp) == EOF) {
      fclose(fp);
      return false;
    }
  }

  fclose(fp);
  return true;
}
