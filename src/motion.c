#include "motion.h"
#include "helpers.h"
#include "utf8.h"

static wchar_t last_find = L'\0';

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

Range motionWordNext(Pos cursor) {
  Pos p = cursor;

  if (classAt(p) == CC_WORD) {
    while (!isBufferEnd(p) && classAt(p) == CC_WORD) {
      p = bufferNextWChar(p);
    }
  }

  while (!isBufferEnd(p) && classAt(p) != CC_WORD) {
    p = bufferNextWChar(p);
  }

  return (Range){cursor, p, false, true};
}

Range motionWordPrev(Pos cursor) {
  Pos p = cursor;

  // if at start of word, go to prev's end

  if (classAt(bufferPrevWChar(p)) != CC_WORD || classAt(p) != CC_WORD) {
    p = bufferPrevWChar(p);
    while (!isBufferStart(p) && classAt(p) != CC_WORD) {
      p = bufferPrevWChar(p);
    }
  }

  if (classAt(p) == CC_WORD) {
    while (!isBufferStart(p) && classAt(bufferPrevWChar(p)) == CC_WORD) {
      p = bufferPrevWChar(p);
    }
  }

  return (Range){cursor, p, false, true};
}

Range motionNextWordEnd(Pos cursor) {
  Pos p = cursor;

  if (classAt(bufferNextWChar(p)) != CC_WORD) {
    p = motionWordNext(p).end;
  }

  while (!isBufferEnd(p) && classAt(bufferNextWChar(p)) == CC_WORD)
    p = bufferNextWChar(p);

  return (Range){cursor, p, true, true};
}

Range motionFindNext(Pos cursor, wchar_t wc, bool until) {
  Pos p = cursor;

  if (wc == L'\0')
    wc = last_find;
  else
    last_find = wc;

  while (!isBufferEnd(p)) {
    p = bufferNextWChar(p);
    if (bufferGetWChar(p) == wc) {
      if (until) {
        // 't': position just before the found character
        p = bufferPrevWChar(p);
      }
      return (Range){cursor, p, true, true};
    }
  }

  return INVALID_RANGE;
}

Range motionFindPrev(Pos cursor, wchar_t wc, bool until) {
  Pos p = cursor;

  if (wc == L'\0')
    wc = last_find;
  else
    last_find = wc;

  while (!isBufferStart(p)) {
    p = bufferPrevWChar(p);
    if (bufferGetWChar(p) == wc) {
      if (until) {
        // 'T': position just after the found character
        p = bufferNextWChar(p);
      }
      return (Range){cursor, p, true, true};
    }
  }
  return (Range){cursor, cursor, true, true};
}

Range motionFirstGraph(Pos cursor) {
  Pos p = cursor;
  p.col = 0;
  p = motionWordNext(p).end;

  return (Range){cursor, p, false, true};
}

Range motionLineEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, bufferLineLength(cursor.row)};
  return (Range){start, end, true, true};
}

Range motionLineStart(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, 0};
  return (Range){start, end, true, true};
}

Range motionLeft(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, cursor.col - 1};
  return (Range){end, end, true, true};
}

Range motionRight(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row, cursor.col + 1};
  return (Range){end, end, true, true};
}

Range motionUp(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row - 1, cursor.col};
  return (Range){end, end, true, true};
}

Range motionDown(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){cursor.row + 1, cursor.col};
  return (Range){end, end, true, true};
}

Range motionFileStart(Pos cursor) {
  Pos start = cursor;
  Pos end = (Pos){0, 0};
  return (Range){start, end, true, true};
}

Range motionFileEnd(Pos cursor) {
  Pos start = cursor;
  Pos end = bufferEnd();
  return (Range){start, end, true, true};
}

// textobjects diw daw etc
Range motionWordInner(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;

  CharClass type = classAt(cursor);

  while (!isBufferStart(start) && classAt(bufferPrevWChar(start)) == type) {
    start = bufferPrevWChar(start);
  }

  while (!isBufferEnd(end) && classAt(bufferNextWChar(end)) == type) {

    end = bufferNextWChar(end);
  }

  log("INNER: %i,%i - %i,%i ,%i", start.row, start.col, end.row, end.col);
  return (Range){start, end, true, true};
}

// inside a word -> remove it and all whitespace after
// on space ->  remove all whitespace and next word
Range motionWordAround(Pos cursor) {
  Pos start = cursor;
  Pos end = cursor;

  CharClass type = classAt(cursor);

  while (!isBufferStart(start) && classAt(bufferPrevWChar(start)) == type) {
    start = bufferPrevWChar(start);
  }

  while (!isBufferEnd(end) && classAt(end) == type || classAt(bufferNextWChar(end)) != type) {
    end = bufferNextWChar(end);
  }

  log("AROUND: %i,%i - %i,%i ,%i", start.row, start.col, end.row, end.col);
  return (Range){start, end, true, true};
}

Range motionMatchingSymbol(Pos cursor) {
  Pos end = cursor;
  wchar_t original = bufferGetWChar(cursor);

  if (original > 255)
    return;

  wchar_t matching = (wchar_t)getMatchingSymbol(original);
  bool forward = original < matching; // ascii values, closing ones always later

  bool (*stepEndFn)(Pos) = forward ? isBufferEnd : isBufferStart;
  Pos (*stepFn)(Pos) = forward ? bufferNextWChar : bufferPrevWChar;
  int skip = 0;

  while (!stepEndFn(end)) {
    end = stepFn(end);

    if (bufferGetWChar(end) == original) {
      skip++;
    }

    if (bufferGetWChar(end) == matching) {
      if (skip <= 0)
        return (Range){cursor, end, true, true};
      else
        skip--;
    }
  }

  return INVALID_RANGE;
}

Range motionCharAround(Pos cursor, wchar_t wc) {
  Pos start = cursor;
  Pos end = cursor;

  // if not at the [ then look forwad and then back for one
  if (bufferGetWChar(start) != wc) {
    Range range;
    range = motionFindNext(start, wc, false);

    if (!range.valid)
      range = motionFindPrev(start, wc, false);

    if (!range.valid)
      return INVALID_RANGE;
    else
      start = range.end;
  }

  end = motionMatchingSymbol(start).end;

  log("CHAR AROUND: %i,%i - %i,%i", start.row, start.col, end.row, end.col);
  return (Range){start, end, true, true};
}

Range motionCharInner(Pos cursor, wchar_t wc) {
  Range out = motionCharAround(cursor, wc);
  out.start.col++;

  if (out.end.col == 0) {
    out.end.row--;
    out.end.col == bufferLineLength(out.end.row);
  } else {
    out.end.col--;
  }

  return out;
}