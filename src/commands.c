#include "buffer.h"
#include "cursor.h"
#include <stddef.h>

void N_newline() {
  Buffer_insert_line(Buffer_get()->line_count, NULL);
  cur_move(0, Buffer_get()->line_count);
}
