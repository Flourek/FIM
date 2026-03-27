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

// keyType is the return of wget_wch (OK or KEY_CODE_YES), ch is the character/key.
// static void waitForInput(int *keyTypeOut, wint_t *chOut) {
static wint_t waitForInput() {
  renderSetCursorStyle(CURSOR_STYLE_UNDERSCORE);
  wint_t ch;
  int keyType = renderGetInput(state.ctx, &ch);
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  return ch;
  // if (keyTypeOut)
  //   *keyTypeOut = keyType;
  // if (chOut)
  //   *chOut = ch;
}

Range handleMotion(int input, Pos cur) {
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
    out = motionFind(cur, (wchar_t)waitForInput(), DIR_FORWARD);
    break;
  case N_FIND_PREV:
    out = motionFind(cur, (wchar_t)waitForInput(), DIR_BACKWARD);
    break;
  case N_FIND_REPEAT:
    out = motionFind(cur, L'\0', DIR_FORWARD);
    break;
  case N_WORD_PREV:
    out = motionWord(cur, DIR_BACKWARD);
    break;
  case N_WORD_END:
    out = motionNextWordEnd(cur);
    break;
  case N_WORD_NEXT:
    out = motionWord(cur, DIR_FORWARD);
    break;
  default:
    return (Range){cur, cur};
  }

  return out;
}

bool nDelete(Range range) {

  range = bufferNormalizeRange(range);
  bufferDeleteRange(range);
  curMove(range.start);
  return false;
}

void waitForMotion() {
  int keyType;
  wint_t ch = waitForInput();

  int input = (int)ch;
  if (input == I_LEAVE_INSERT)
    return;

  Range range = handleMotion(input, cursor);
  // log("Range: <%i,%i> - <%i,%i>", range.start.row, range.start.col, range.end.row, range.end.col);

  Range truncated = range;
  if (!range.inclusive) {
    truncated.end.col--;
  }

  if (range.start.row != range.end.row) {
    // return;
    // range.end.row = range.start.row;
    // range.end.col = bufferLineLength(range.start.row);
  }

  nDelete(truncated);
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
  case N_DELETE:
    waitForMotion();
    break;
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
    int kt;
    wint_t ch;
    waitForInput(&kt, &ch);
    // Use the real Unicode codepoint for replace
    nReplace(ch);
  } break;
  case N_CHUJ:
    int macro[7] = {'i', KEY_END, ' ', 'c', 'h', 'u', 'j'};
    inputHandleMacro(macro, 7);
    break;
  default:
    Range range = handleMotion(input, cursor);
    curMove(range.end);
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
    // inputHandle(macro[i]);
  }
}

bool inputHandle(int keyType, wint_t ch) {
  wint_t input = ch;
  state.last_key = ch;

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

  return false;
}
