#ifndef MOTION_H
#define MOTION_H

#include "buffer.h"

typedef enum {
  CC_WORD,  // letters, digits, underscore
  CC_SPACE, // spaces, tabs, newline
  CC_OTHER  // punctuation: . , ; ( ) etc
} CharClass;

typedef enum {
  DIR_FORWARD,
  DIR_BACKWARD
} Direction;

Range motionWord(Pos cursor, Direction dir);

Range motionNextWordEnd(Pos cursor);
Range motionFirstGraph(Pos cursor);
Range motionPrevWordEnd(Pos cursor);

Range motionLineEnd(Pos cursor);
Range motionLineStart(Pos cursor);
Range motionLeft(Pos cursor);
Range motionRight(Pos cursor);
Range motionUp(Pos cursor);
Range motionDown(Pos cursor);
Range motionFileStart(Pos cursor);
Range motionFileEnd(Pos cursor);

Range motionFind(Pos cursor, wchar_t wc, Direction dir);

#endif
