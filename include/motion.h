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

Range motionWordNext(Pos cursor);
Range motionWordPrev(Pos cursor);

Range motionNextWordEnd(Pos cursor);
Range motionFirstGraph(Pos cursor);
Range motionPrevWordEnd(Pos cursor);

Range motionLine(Pos cursor);
Range motionLineEnd(Pos cursor);
Range motionLineStart(Pos cursor);
Range motionLeft(Pos cursor);
Range motionRight(Pos cursor);
Range motionUp(Pos cursor);
Range motionDown(Pos cursor);
Range motionFileStart(Pos cursor);
Range motionFileEnd(Pos cursor);

Range motionFindNext(Pos cursor, wchar_t wc, bool until);
Range motionFindPrev(Pos cursor, wchar_t wc, bool until);

Range motionMatchingSymbol(Pos cursor);
Range motionBracketAround(Pos cursor, wchar_t wc);
Range motionBracketInner(Pos cursor, wchar_t wc);
Range motionWordInner(Pos cursor);
Range motionWordAround(Pos cursor);
#endif
