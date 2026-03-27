#ifndef COMMAND_H
#define COMMAND_H

#include "linebuffer.h"

typedef struct {
  char *name;
  int min_abb;
  void (*run)();
} Command;

void commandInsertWChar(wchar_t wc);
void commandBackspace(void);
const char *commandGetBuffer(void);
void commandClear(void);
LineBuffer *commandGetLineBuffer(void);
void commandRun(void);

#endif