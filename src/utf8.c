#include "utf8.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// Basic UTF-8 property:
// 0xxxxxxx : 1 byte (ASCII)
// 110xxxxx : 2 bytes
// 1110xxxx : 3 bytes
// 11110xxx : 4 bytes
// 10xxxxxx : continuation byte

int utf8_char_len(const char *str, int idx) {
  if (!str)
    return 0;
  unsigned char c = (unsigned char)str[idx];

  if (c == 0)
    return 0;
  if (c < 0x80)
    return 1;
  if ((c & 0xE0) == 0xC0)
    return 2;
  if ((c & 0xF0) == 0xE0)
    return 3;
  if ((c & 0xF8) == 0xF0)
    return 4;

  // If we're inside a sequence (shouldn't happen if called at start), return 1 to advance safely
  return 1;
}

int utf8_next(const char *str, int idx) {
  if (!str || idx < 0)
    return 0;
  int len = (int)strlen(str);
  if (idx >= len)
    return len;

  return idx + utf8_char_len(str, idx);
}

int utf8_prev(const char *str, int idx) {
  if (!str || idx <= 0)
    return 0;

  int i = idx - 1;
  // Move back while we're on a continuation byte (10xxxxxx)
  while (i > 0 && ((unsigned char)str[i] & 0xC0) == 0x80) {
    i--;
  }
  return i;
}

int utf8_width(const char *str, int idx) {
  if (!str)
    return 0;

  wchar_t wc;
  int len = utf8_char_len(str, idx);
  if (len == 0)
    return 0;

  // Convert just this character to wide char to check width
  int res = mbtowc(&wc, str + idx, len);
  if (res < 0)
    return 1; // logical fallback

  int w = wcwidth(wc);
  return (w >= 0) ? w : 1;
}

int utf8_encode(char *dest, wint_t ch) {
  if (!dest)
    return 0;

  // wctomb requires state reset potentially, using simple wctomb
  // We assume UTF-8 locale is set.
  int len = wctomb(dest, (wchar_t)ch);
  if (len < 0)
    return 0;
  dest[len] = '\0';
  return len;
}

int utf8_byte_to_column(const char *str, int byte_index) {
  if (!str)
    return 0;

  int col = 0;
  int i = 0;
  while (str[i] && i < byte_index) {
    col += utf8_width(str, i);
    i = utf8_next(str, i);
  }
  return col;
}

int utf8_column_to_byte(const char *str, int target_col) {
  if (!str)
    return 0;

  int col = 0;
  int i = 0;
  while (str[i]) {
    if (col >= target_col)
      return i;

    int w = utf8_width(str, i);
    col += w;
    i = utf8_next(str, i);
  }
  return i;
}
