#include "input.h"
#include "commands.h"
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
#define N_ENTER_INSERT 'i'
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

char waitForInput() {
  renderSetCursorStyle(CURSOR_STYLE_UNDERSCORE);
  char out = getchar();
  renderSetCursorStyle(CURSOR_STYLE_BLOCK);
  return out;
}

static void normalMode(int input) {
  switch (input) {
  case N_ENTER_INSERT:
    state.mode = MODE_INSERT;
    renderSetCursorStyle(CURSOR_STYLE_BAR);
    break;
  case N_MERGELINE:
    nMergeLine();
    break;
  case N_NEWLINE:
    niNewline();
    break;
  case N_REPLACE:
    nReplace(waitForInput());
    break;
  case N_LINE_END:
    niLineEnd();
    break;
  case N_LINE_START:
    niLineStart();
    break;
  case N_CURSOR_LEFT:
    niCursorLeft();
    break;
  case N_CURSOR_RIGHT:
    niCursorRight();
    break;
  case N_CURSOR_UP:
    niCursorUp();
    break;
  case N_FIRST_GRAPH:
    nFirstGraph();
    break;
  case N_WORD_PREV:
    nWordPrev();
    break;
  case N_WORD_END:
    nWordEnd();
    break;
  case N_WORD_NEXT:
    nWordNext();
    break;
  case N_CHUJ:
    int macro[5] = {'i', KEY_END, 'h', 'u', 'j'};
    inputHandleMacro(macro, 5);
    break;
  case N_CURSOR_DOWN:
    niCursorDown();
    break;
  default:
    break;
  }
}

static void insertMode(int input) {
  switch (input) {
  case I_LEAVE_INSERT:
    state.mode = MODE_NORMAL;
    renderSetCursorStyle(CURSOR_STYLE_BLOCK);
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
    niCursorLeft();
    break;
  case I_NEWLINE:
    niNewline();
    break;
  case I_CURSOR_RIGHT:
    niCursorRight();
    break;
  case I_CURSOR_UP:
    niCursorUp();
    break;
  case I_CURSOR_DOWN:
    niCursorDown();
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
