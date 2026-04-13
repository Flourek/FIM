#include "motion.h"
#include "buffer.h"
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
  return INVALID_RANGE;
}

Range motionFirstGraph(Pos cursor) {
  Pos p = cursor;
  p.col = 0;
  p = motionWordNext(p).end;

  return (Range){cursor, p, false, true};
}

Range motionLine(Pos cursor) {
  Pos start = (Pos){cursor.row, 0};
  Pos end = (Pos){cursor.row, bufferLineLength(cursor.row)};
  return (Range){start, end, true, true};
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
  Pos start = cursor;

  while (!isBufferEnd(start)) {
    if (isBracketSymbol(bufferGetWChar(start))) {
      break;
    }
    start = bufferNextWChar(start);
  }

  wchar_t original = bufferGetWChar(start);

  wchar_t matching = (wchar_t)getMatchingSymbol(original);
  bool forward = original < matching; // ascii values, closing ones always later

  bool (*stepEndFn)(Pos) = forward ? isBufferEnd : isBufferStart;
  Pos (*stepFn)(Pos) = forward ? bufferNextWChar : bufferPrevWChar;
  int skip = 0;
  Pos end = start;

  while (!stepEndFn(end)) {
    end = stepFn(end);

    if (bufferGetWChar(end) == original) {
      skip++;
    }

    if (bufferGetWChar(end) == matching) {
      if (skip <= 0) {
        // log("CHAR AROUND: %i,%i - %i,%i", start.row, start.col, end.row, end.col);
        return (Range){start, end, true, true};
      } else
        skip--;
    }
  }

  return INVALID_RANGE;
}

Range motionBracketAround(Pos cursor, wchar_t wc) {
  if (!isBracketSymbol(wc))
    return (Range){0};

  wchar_t open = wc;
  wchar_t close = (wchar_t)getMatchingSymbol(wc);

  Pos pos = cursor;

  // STEP 1: If not on bracket, search backwards for opening
  if (bufferGetWChar(pos) != open && bufferGetWChar(pos) != close) {
    int depth = 0;
    bool found = false;

    while (!isBufferStart(pos)) {
      pos = bufferPrevWChar(pos);
      wchar_t c = bufferGetWChar(pos);

      if (c == open) {
        if (depth == 0) {
          found = true;
          break;
        }
        depth--;
      }

      if (c == close) {
        depth++;
      }
    }

    if (!found) {
      Pos linewisePos = (Pos){cursor.row, 0};
      Range nextSymbol = motionMatchingSymbol(linewisePos);
      pos = nextSymbol.end;

      if (!nextSymbol.valid)
        return INVALID_RANGE;
    }
    log("CHAR AROUND: %i,%i - %i", pos.row, pos.col, found);
  }

  // If we landed on closing, jump to opening

  Pos start = pos;
  Range match = motionMatchingSymbol(start);

  if (!match.valid)
    return INVALID_RANGE;

  Pos end = match.end;

  log("CHAR AROUND: %i,%i - %i,%i", start.row, start.col, end.row, end.col);
  return (Range){start, end, true, true};
}

Range motionBracketInner(Pos cursor, wchar_t wc) {
  Range out = motionBracketAround(cursor, wc);
  out = bufferNormalizeRange(out);
  out.start.col++;

  if (out.end.col == 0) {
    out.end.row--;
    out.end.col == bufferLineLength(out.end.row);
  } else {
    out.end.col--;
  }
  log("CHAR INNER: %i,%i - %i,%i", out.start.row, out.start.col, out.end.row, out.end.col);

  return out;
}