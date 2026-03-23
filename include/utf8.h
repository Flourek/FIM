#ifndef UTF8_H
#define UTF8_H

#include <stddef.h>
#include <wchar.h>

// Returns the index of the start of the next UTF-8 character.
int utf8_next(const char *str, int idx);

// Returns the index of the start of the previous UTF-8 character.
int utf8_prev(const char *str, int idx);

// Returns the byte length of the UTF-8 character starting at str[idx].
int utf8_char_len(const char *str, int idx);

// Returns the display width of the UTF-8 character starting at str[idx].
int utf8_width(const char *str, int idx);

// Converts a wide character (from get_wch) to a UTF-8 string.
// dest must be at least 5 bytes long. Returns number of bytes written.
int utf8_encode(char *dest, wint_t ch);

// Converts byte index to visual column index (handling double-width chars).
int utf8_byte_to_column(const char *str, int byte_index);

// Converts visual column index to byte index (closest match).
int utf8_column_to_byte(const char *str, int column);

#endif
