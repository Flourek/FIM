#ifndef STATE_H
#define STATE_H

#include "render.h"
#define LOG_MESSAGE_CAPACITY 512

typedef enum {
  MODE_NORMAL,
  MODE_INSERT,
  MODE_COMMAND,
  MODE_SEARCH
} EditorMode;

typedef struct {
  EditorMode mode;
  int last_key;
  char key_combo[16];
  char log_message[LOG_MESSAGE_CAPACITY];
  RenderContext *ctx;
} State;

extern State state;
extern const char *ModeNames[];

#endif
