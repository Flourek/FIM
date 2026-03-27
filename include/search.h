#ifndef SEARCH_H
#define SEARCH_H

#include "linebuffer.h"

// Search-mode specific API
void searchInsertWChar(wchar_t wc);
void searchBackspace(void);
const char *searchGetBuffer(void);
void searchClear(void);
LineBuffer *searchGetLineBuffer(void);
void searchRun(void);

#endif
