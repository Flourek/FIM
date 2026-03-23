#ifndef COMMANDS_H
#define COMMANDS_H

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
#endif
