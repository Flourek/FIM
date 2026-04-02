#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
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
#define N_FIND_NEXT 'f'
#define N_FIND_PREV 'F'
#define N_FIND_REPEAT ';'
#define N_UNTIL_NEXT 't'
#define N_UNTIL_PREV 'T'
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
#define N_COMMAND ':'
#define N_SEARCH '/'
#define TEXT_OBJECT_INNER 'i'
#define TEXT_OBJECT_AROUND 'a'
#define N_MATCHING_SYMBOL '%'
// keyType is the return value of wget_wch: OK or KEY_CODE_YES.
bool inputHandle(int keyType, wint_t ch);

#endif
