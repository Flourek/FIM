#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>

char *strdupSafe(const char *s);
void log(const char *fmt, ...);
char getMatchingSymbol(char c);
bool isBracketSymbol(char c);
bool isClosingSymbol(char c);
#endif
