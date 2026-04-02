#include "state.h"

State state = {
    .mode = MODE_NORMAL,
    .last_key = 'E',
    .key_combo = "",
    .log_message = "",
    .ctx = 0,
};

const char *ModeNames[] = {"Normal", "Insert", "Command", "Search"};
