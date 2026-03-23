#ifndef FILES_H
#define FILES_H

#include "buffer.h"
#include <stdbool.h>

bool filesLoadIntoBuffer(const char *path, Buffer *buf);
bool filesSaveFromBuffer(const char *path, const Buffer *buf);

#endif
