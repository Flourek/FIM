#include "input.h"
#include "commands.h"
#include "files.h"
#include "helpers.h"
#include "motion.h"
#include "render.h"
#include "state.h"
#include <ncurses.h>
#include <wchar.h>

#define N_CURSOR_RIGHT 'l'
#define N_CURSOR_LEFT 'h'
#define N_CURSOR_UP 'k'
#define N_CURSOR_DOWN 'j'
#define I_CURSOR_RIGHT KEY_RIGHT
#define I_CURSOR_LEFT KEY_LEFT
#define I_CURSOR_UP KEY_UP
#define I_CURSOR_DOWN KEY_DOWN
#define N_QUIT 'q'
#define N_APPEND 'a'
#define N_INSERT 'i'
#define I_LEAVE_INSERT 27
#define I_BACKSPACE KEY_BACKSPACE
#define N_NEWLINE 'K'
#define N_MERGELINE 'J'
#define N_LINE_START '0'
#define N_LINE_END '$'
#define I_LINE_START KEY_HOME
#define I_LINE_END KEY_END
#define I_NEWLINE 10
#define N_CHUJ 'c'
#define N_WORD_NEXT 'w'
#define N_WORD_PREV 'b'
#define N_WORD_END 'e'
#define N_FIRST_GRAPH '^'
#define N_REPLACE 'r'
#define N_DELETE_CHAR 'x'
#define N_DELETE 'd'
#define N_SUBSTITUTE 's'
#define N_SAVE 19
#define N_LOAD 23
#define N_FILE_END 'G'
#define N_G_KEY 'g'
#define N_G_FILE_START 'g'

#include "utf8.h"

// keyType is the return of wget_wch (OK or KEY_CODE_YES), ch is the character/key.
static void waitForInput(int *keyTypeOut, wint_t *chOut) {
  renderSetCursorStyle(CURSOR_STYLE_UNDERSCORE);
  wint_t ch;
  int keyType = renderGetInput(state.ctx, &ch);
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  if (keyTypeOut)
    *keyTypeOut = keyType;
  if (chOut)
    *chOut = ch;
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
  case N_WORD_PREV:
    out = motionWordPrevStart(cur);
    break;
  case N_WORD_END:
    out = motionWordNextEnd(cur);
    break;
  case N_WORD_NEXT:
    out = motionWordNextStart(cur);
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
  wint_t ch;
  waitForInput(&keyType, &ch);

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

static void normalMode(int keyType, int input) {
  switch (input) {
  case N_INSERT:
    nInsert();
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

static void insertMode(int keyType, int input) {
  // Distinguish between printable chars (keyType == OK) and special keys
  if (input == I_LEAVE_INSERT) {
  }

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
  int input = (int)ch;

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
  default:
    break;
  }

  return false;
}
