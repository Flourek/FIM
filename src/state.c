#include "state.h"

State state = {
    .mode = MODE_NORMAL,
    .last_key = 'E',
    .log_message = "",
};

const char *ModeNames[] = {"Normal", "Insert", "Visual", "Visual_line"};
