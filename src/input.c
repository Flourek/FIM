#include "input.h"
#include "actions.h"
#include "command.h"
#include "files.h"
#include "helpers.h"
#include "motion.h"
#include "render.h"
#include "search.h"
#include "state.h"
#include <ncurses.h>
#include <wchar.h>

#include "utf8.h"

#include <string.h>

static void keyComboClear() {
  state.key_combo[0] = '\0';
}

static void keyComboAppend(wint_t ch) {
  if (ch <= 0 || ch > 127)
    return;

  size_t len = strlen(state.key_combo);
  if (len + 1 >= sizeof(state.key_combo))
    return;

  state.key_combo[len] = (char)ch;
  state.key_combo[len + 1] = '\0';
}

static wint_t waitForInput() {
  renderSetCursorStyle(CURSOR_STYLE_UNDERSCORE);
  wint_t ch;
  int keyType = renderGetInput(state.ctx, &ch);
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  keyComboAppend(ch);
  return ch;
}

Range handleMotion(wint_t input, Pos cur) {
  Range out;

  switch (input) {
  case N_LINE_END:
    out = motionLineEnd(cur);
    break;
  case N_LINE_START:
    out = motionLineStart(cur);
    break;
  case N_CURSOR_LEFT:
    out = motionLeft(cur);
    break;
  case N_CURSOR_RIGHT:
    out = motionRight(cur);
    break;
  case N_CURSOR_UP:
    out = motionUp(cur);
    break;
  case N_CURSOR_DOWN:
    out = motionDown(cur);
    break;
  case N_G_KEY:
    out = motionFileStart(cur);
    break;
  case N_FILE_END:
    out = motionFileEnd(cur);
    break;
  case N_FIRST_GRAPH:
    out = motionFirstGraph(cur);
    break;
  case N_FIND_NEXT:
    out = motionFindNext(cur, (wchar_t)waitForInput(), false);
    break;
  case N_FIND_PREV:
    out = motionFindPrev(cur, (wchar_t)waitForInput(), false);
    break;
  case N_FIND_REPEAT:
    out = motionFindNext(cur, L'\0', false);
    break;
  case N_UNTIL_NEXT:
    out = motionFindNext(cur, (wchar_t)waitForInput(), true);
    break;
  case N_UNTIL_PREV:
    out = motionFindPrev(cur, (wchar_t)waitForInput(), true);
    break;
  case N_WORD_PREV:
    out = motionWordPrev(cur);
    break;
  case N_MATCHING_SYMBOL:
    out = motionMatchingSymbol(cur);
    break;
  case N_WORD_END:
    out = motionNextWordEnd(cur);
    break;
  case N_WORD_NEXT:
    out = motionWordNext(cur);
    break;
  default:
    return INVALID_RANGE;
  }

  return out;
}

Range handleTextObject(bool inner) {
  wint_t key = waitForInput();

  if (key == I_LEAVE_INSERT)
    return INVALID_RANGE;

  if (key == N_WORD_NEXT) {
    if (inner) {
      return motionWordInner(cursor);
    } else {
      return motionWordAround(cursor);
    }
  }

  if (inner)
    return motionBracketInner(cursor, key);
  else
    return motionBracketAround(cursor, key);

  return INVALID_RANGE;
}

Range waitForMotion(wint_t actionKey) {
  wint_t input = waitForInput();

  // cc dd..
  if (actionKey == input)
    return motionLine(cursor);

  switch (input) {
  case I_LEAVE_INSERT:
    return INVALID_RANGE;
  case TEXT_OBJECT_INNER:
    return handleTextObject(true);
  case TEXT_OBJECT_AROUND:
    return handleTextObject(false);
  default:
    return handleMotion(input, cursor);
    break;
  }

  // log("Range: <%i,%i> - <%i,%i>", range.start.row, range.start.col, range.end.row, range.end.col);
}

static void linebufferInput(int keyType, wint_t input) {
  LineBuffer *buf = 0;
  void (*runFn)(void) = 0;

  if (state.mode == MODE_COMMAND) {
    buf = commandGetLineBuffer();
    runFn = commandRun;
  } else if (state.mode == MODE_SEARCH) {
    buf = searchGetLineBuffer();
    runFn = searchRun;
  } else {
    return;
  }

  switch (input) {
  case I_LEAVE_INSERT:
    state.mode = MODE_NORMAL;
    lineBufferClear(buf);
    break;
  case I_BACKSPACE:
    lineBufferBackspace(buf);
    break;
  case I_NEWLINE:
    state.mode = MODE_NORMAL;
    runFn();
    break;
  default:
    if (keyType == OK) {
      lineBufferInsertWChar(buf, (wchar_t)input);
    }
    break;
  }
}

static void normalMode(int keyType, wint_t input) {
  switch (input) {
  case N_INSERT:
    nInsert();
    break;
  case N_COMMAND:
    state.mode = MODE_COMMAND;
    break;
  case N_SEARCH:
    state.mode = MODE_SEARCH;
    searchClear();
    break;
  case N_DELETE: {
    Range range = waitForMotion(input);
    nDelete(range);
    break;
  }
  case N_CHANGE: {
    Range range = waitForMotion(input);
    nChange(range);
    break;
  }
  case N_MERGELINE:
    nMergeLine();
    break;
  case N_APPEND:
    nAppend();
    break;
  case N_NEWLINE:
    niNewline();
    break;
  case N_LOAD:
    filesLoadIntoBuffer("./chuj", bufferGet());
    break;
  case N_SAVE:
    filesSaveFromBuffer("./chuj", bufferGet());
    break;
  case N_SUBSTITUTE:
    nSubstitute();
    break;
  case N_DELETE_CHAR:
    nDeleteChar();
    break;
  case N_REPLACE: {
    wint_t ch = waitForInput();
    nReplace(ch);
  } break;
  case N_CHUJ:
    int macro[7] = {'i', KEY_END, ' ', 'c', 'h', 'u', 'j'};
    inputHandleMacro(macro, 7);
    break;
  default:
    Range range = handleMotion(input, cursor);
    if (range.valid) {
      curMove(range.end);
    }
    break;
  }
}

static void insertMode(int keyType, wint_t input) {
  // Distinguish between printable chars (keyType == OK) and special keys

  if (keyType == KEY_CODE_YES) {
    // Function / control keys
    switch (input) {
    case I_LINE_END:
      niLineEnd();
      return;
    case I_LINE_START:
      niLineStart();
      return;
    case I_BACKSPACE:
      iBackspace();
      return;
    case I_CURSOR_LEFT:
      curMove(motionLeft(cursor).end);
      return;
    case I_NEWLINE:
      niNewline();
      return;
    case I_CURSOR_RIGHT:
      curMove(motionRight(cursor).end);
      return;
    case I_CURSOR_UP:
      curMove(motionUp(cursor).end);
      return;
    case I_CURSOR_DOWN:
      curMove(motionDown(cursor).end);
      return;
    default:
      // Unhandled function key in insert mode: ignore
      break;
    }
  } else {
    // Function / control keys
    switch (input) {
    case I_LEAVE_INSERT:
      iLeaveInsert();
      return;
    case I_NEWLINE:
      niNewline();
      return;
    default:
      // Unhandled function key in insert mode: ignore
      break;
    }
  }

  iInsertCharacter((wint_t)input);
}

void inputHandleMacro(int *macro, int len) {
  for (size_t i = 0; i < len; i++) {
    inputHandle(0, macro[i]);
  }
}

bool inputHandle(int keyType, wint_t ch) {
  wint_t input = ch;
  state.last_key = ch;
  keyComboAppend(ch);

  if (state.mode == MODE_NORMAL && input == N_QUIT) {
    return true;
  }

  switch (state.mode) {
  case MODE_INSERT:
    insertMode(keyType, input);
    break;
  case MODE_NORMAL:
    normalMode(keyType, input);
    break;
  case MODE_COMMAND:
  case MODE_SEARCH:
    linebufferInput(keyType, input);
    break;
  default:
    break;
  }

  keyComboClear();

  return false;
}
