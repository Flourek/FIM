#include "motion.h"

Range motionWordNextStart(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  end = bufferTraverse(end, bufferEnd(), bufferIsCharBlank);
  end = bufferTraverse(end, bufferEnd(), bufferIsCharGraph);
  return (Range){start, end, false};
}

Range motionWordPrevStart(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  Pos prevPos = (Pos){cursor.row, cursor.col - 1};

  if (bufferIsCharBlank(prevPos)) {
    Range prevEnd = motionPrevWordEnd(end);
    end = prevEnd.end;
  }

  Range wordStart = motionWordStart(end);
  end = wordStart.end;

  return (Range){start, end, true};
}

Range motionWordNextEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  Pos nextPos = (Pos){cursor.row, cursor.col + 1};

  if (bufferIsCharGraph(start) && !bufferIsCharBlank(nextPos)) {
    end = motionWordEnd(end).end;
  } else {
    end = motionWordNextStart(end).end;
    end = motionWordEnd(end).end;
  }

  return (Range){start, end, true};
}

Range motionFirstGraph(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  end.col = 0;
  end = bufferTraverse(end, bufferEnd(), bufferIsCharGraph);
  return (Range){start, end, true};
}

Range motionWordStart(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  end = bufferTraverse(end, bufferStart(), bufferIsCharBlank);
  end = bufferTraverse(end, bufferEnd(), bufferIsCharGraph);
  return (Range){start, end, true};
}

Range motionPrevWordEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  end = bufferTraverse(end, bufferStart(), bufferIsCharBlank);
  end = bufferTraverse(end, bufferStart(), bufferIsCharGraph);
  return (Range){start, end, true};
}

Range motionWordEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;
  end = bufferTraverse(end, bufferEnd(), bufferIsCharBlank);
  end = bufferTraverse(end, bufferStart(), bufferIsCharGraph);
  return (Range){start, end, false};
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
