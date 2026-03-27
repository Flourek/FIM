#include "motion.h"
#include "helpers.h"
#include "utf8.h"

static wchar_t last_find = '\0';

CharClass class(wchar_t wc) {
  if (iswalnum(wc) || wc == '_')
    return CC_WORD;
  else if (iswspace(wc))
    return CC_SPACE;
  else
    return CC_OTHER;
}

CharClass classAt(Pos pos) {
  return class(bufferGetWChar(pos));
}

Pos step(Pos p, Direction dir) {
  return (dir == DIR_FORWARD)
             ? bufferNextWChar(p)
             : bufferPrevWChar(p);
}

bool isStepEnd(Pos p, Direction dir) {
  if (dir == DIR_FORWARD)
    isBufferEnd(p);
  else
    isBufferStart(p);
}

Range motionWord(Pos cursor, Direction dir) {
  Pos p = cursor;
  wchar_t wc;

  // Step once if going backward (important!)
  if (dir == DIR_BACKWARD)
    p = step(p, dir);

  // Skip non-word
  // Step 1: if currently in a word → skip current word
  if (classAt(p) == CC_WORD) {
    while (!isStepEnd(p, dir) && classAt(p) == CC_WORD) {
      p = step(p, dir);
    }
  }

  // Skip word
  while (!isStepEnd(p, dir) && classAt(p) != CC_WORD) {
    p = step(p, dir);
  }

  return (Range){cursor, p, false};
}

Range motionNextWordEnd(Pos cursor) {

  Pos p = cursor;

  // If at end of word go bufferNextWChar
  if (classAt(bufferNextWChar(p)) != CC_WORD) {
    p = motionWord(p, DIR_FORWARD).end;
  }

  while (!isBufferEnd(p) && classAt(bufferNextWChar(p)) == CC_WORD)
    p = bufferNextWChar(p);

  return (Range){cursor, p, false};
}

Range motionFind(Pos cursor, wchar_t wc, Direction dir) {
  Pos p = cursor;

  if (wc == '\0')
    wc = last_find;
  else
    last_find = wc;

  while (!isStepEnd(p, dir)) {
    p = step(p, dir);
    if (bufferGetWChar(p) == wc) {
      return (Range){cursor, p, false};
    }
  }
  return (Range){cursor, cursor, false};
}

Range motionFirstGraph(Pos cursor) {

  Pos p = cursor;
  p.col = 0;
  p = motionWord(p, DIR_FORWARD).end;

  return (Range){cursor, p, false};
}

Range motionLineEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, bufferLineLength(cursor.row)};
  return (Range){start, end, true};
}

Range motionLineStart(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, 0};
  return (Range){start, end, true};
}

Range motionLeft(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, cursor.col - 1};
  return (Range){end, end, true};
}

Range motionRight(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, cursor.col + 1};
  return (Range){end, end, true};
}

Range motionUp(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row - 1, cursor.col};
  return (Range){end, end, true};
}

Range motionDown(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row + 1, cursor.col};
  return (Range){end, end, true};
}

Range motionFileStart(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){0, 0};
  return (Range){start, end, true};
}

Range motionFileEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = bufferEnd();
  return (Range){start, end, true};
}