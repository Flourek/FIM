#include "input.h"
#include "commands.h"
#include "helpers.h"
#include "motion.h"
#include "render.h"
#include "state.h"
#include <ncurses.h>

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

int waitForInput() {
  renderSetCursorStyle(CURSOR_STYLE_UNDERSCORE);
  int out = renderGetInput(state.ctx);
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  return out;
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
    break;
  }

  return out;
}

bool nDelete(Range range) {
  if (range.start.row != range.end.row) {
    log("%s ", "chuj");
    return true;
  }

  bufferDeleteRange(range);
  return false;
}

void waitForMotion() {
  int input = waitForInput();
  if (input == I_LEAVE_INSERT)
    return;

  Range range = handleMotion(input, cursor);

  if (range.start.col > range.end.col) {
    Pos tmp = range.start;
    range.start = range.end;
    range.end = tmp;
  }

  Range truncated = range;
  if (!range.inclusive) {
    truncated.end.col--;
  }

  nDelete(truncated);
  curMove(range.start);
}

static void normalMode(int input) {
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
  case N_SUBSTITUTE:
    nSubstitute();
    break;
  case N_DELETE_CHAR:
    nDeleteChar();
    break;
  case N_REPLACE:
    nReplace((char)waitForInput());
    break;
  case N_CHUJ:
    int macro[5] = {'i', KEY_END, 'h', 'u', 'j'};
    inputHandleMacro(macro, 5);
    break;
  default:
    Range range = handleMotion(input, cursor);
    curMove(range.end);
    break;
  }
}

static void insertMode(int input) {
  switch (input) {
  case I_LEAVE_INSERT:
    iLeaveInsert();
    break;
  case I_LINE_END:
    niLineEnd();
    break;
  case I_LINE_START:
    niLineStart();
    break;
  case I_BACKSPACE:
    iBackspace();
    break;
  case I_CURSOR_LEFT:
    curMove(motionLeft(cursor).end);
    break;
  case I_NEWLINE:
    niNewline();
    break;
  case I_CURSOR_RIGHT:
    curMove(motionRight(cursor).end);
    break;
  case I_CURSOR_UP:
    curMove(motionUp(cursor).end);
    break;
  case I_CURSOR_DOWN:
    curMove(motionDown(cursor).end);
    break;
  default:
    if (input <= 255) {
      iInsertCharacter((char)input);
    }
    break;
  }
}

void inputHandleMacro(int *macro, int len) {
  for (size_t i = 0; i < len; i++) {
    inputHandle(macro[i]);
  }
}

bool inputHandle(int input) {
  if (state.mode == MODE_NORMAL && input == N_QUIT) {
    return true;
  }

  log("");

  switch (state.mode) {
  case MODE_INSERT:
    insertMode(input);
    break;
  case MODE_NORMAL:
    normalMode(input);
    break;
  default:
    break;
  }

  return false;
}
