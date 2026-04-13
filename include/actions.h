#ifndef ACTIONS_H
#define ACTIONS_H
#include "buffer.h"

void niNewline();
void nReplace(wint_t ch);
void nDeleteChar();
void niLineEnd();
void niLineStart();
void niCursorLeft();
void niCursorRight();
void niCursorUp();
void niCursorDown();
void nQuit();
void nAppend();
void nInsert();
void iInsertCharacter(wint_t ch);
void iLeaveInsert();
void nFirstGraph();
void nWordPrev();
void nWordEnd();
void nWordNext();
void nMergeLine();
void nSubstitute();
void iBackspace();
bool nDelete(Range range);
bool nChange(Range range);
#endif
