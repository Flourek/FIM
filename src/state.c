#include "state.h"

State state = {
    .mode = MODE_NORMAL,
    .last_key = 'E',
    .log_message = "",
    .ctx = 0,
};

const char *ModeNames[] = {"Normal", "Insert", "Visual", "Visual_line"};
