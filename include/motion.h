#ifndef MOTION_H
#define MOTION_H

#include "buffer.h"

Range motionWordNextStart(Pos cursor);
Range motionWordPrevStart(Pos cursor);
Range motionWordNextEnd(Pos cursor);
Range motionFirstGraph(Pos cursor);
Range motionWordStart(Pos cursor);
Range motionPrevWordEnd(Pos cursor);
Range motionWordEnd(Pos cursor);
Range motionLineEnd(Pos cursor);
Range motionLineStart(Pos cursor);
Range motionLeft(Pos cursor);
Range motionRight(Pos cursor);
Range motionUp(Pos cursor);
Range motionDown(Pos cursor);
#endif
