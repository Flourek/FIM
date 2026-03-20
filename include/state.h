#ifndef STATE_H
#define STATE_H

#include "render.h"
#define LOG_MESSAGE_CAPACITY 512

typedef enum {
  MODE_NORMAL,
  MODE_INSERT
} EditorMode;

typedef struct {
  EditorMode mode;
  int last_key;
  char log_message[LOG_MESSAGE_CAPACITY];
  RenderContext *ctx;
} State;

extern State state;
extern const char *ModeNames[];

#endif
