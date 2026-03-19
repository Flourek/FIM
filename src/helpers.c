#include "helpers.h"
#include "state.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdupSafe(const char *s) {
  if (!s)
    return NULL;

  size_t len = strlen(s);
  char *out = malloc(len + 1);
  if (!out)
    return NULL;

  memcpy(out, s, len + 1);
  return out;
}

void log(const char *fmt, ...) {
  if (!fmt) {
    state.log_message[0] = '\0';
    return;
  }

  va_list args;
  va_start(args, fmt);
  vsnprintf(state.log_message, sizeof(state.log_message), fmt, args);
  va_end(args);
}
