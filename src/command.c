#include "buffer.h"
#include "cursor.h"
#include "files.h"
#include "helpers.h"
#include <stdio.h>
#include <string.h>

void cmdWrite(const char *args) {
  char path[100];
  snprintf(path, sizeof(path), "./save/%s", *args ? args : "DEFAULT");
  filesSaveFromBuffer(path, bufferGet());
  log("Wrote to %s %i", path, *args);
}

void cmdEdit(const char *args) {
  char path[100];
  const char *filename;
  if (args && *args)
    filename = args;
  else
    filename = "DEFAULT";
  bufferClear();
  snprintf(path, sizeof(path), "./save/%s", filename);
  filesLoadIntoBuffer(path, bufferGet());
  curReset();
  log("Loaded: %s", path);
}