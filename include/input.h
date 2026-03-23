#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include <wchar.h>

// keyType is the return value of wget_wch: OK or KEY_CODE_YES.
bool inputHandle(int keyType, wint_t ch);

#endif
