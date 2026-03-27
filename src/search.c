#include "search.h"

static LineBuffer searchBuffer = {"", 0};

void searchInsertWChar(wchar_t wc) {
  lineBufferInsertWChar(&searchBuffer, wc);
}

void searchBackspace(void) {
  lineBufferBackspace(&searchBuffer);
}

const char *searchGetBuffer(void) {
  return lineBufferCStr(&searchBuffer);
}

void searchClear(void) {
  lineBufferClear(&searchBuffer);
}

LineBuffer *searchGetLineBuffer(void) {
  return &searchBuffer;
}

void searchRun(void) {
  // TODO: execute search based on searchGetBuffer()
}
