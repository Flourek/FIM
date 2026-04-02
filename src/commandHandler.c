#include "command.h"
#include "helpers.h"
#include <stdbool.h>
#include <string.h>
// Forward declarations for concrete command implementations.

static LineBuffer commandBuffer = {"", 0};

static Command cmdTable[] = {
    {"edit", 1, cmdEdit},
    {"write", 1, cmdWrite},
};
static int cmdTableLen = sizeof(cmdTable) / sizeof(cmdTable[0]);

LineBuffer *commandGetLineBuffer(void) {
  return &commandBuffer;
}

void commandRun(void) {
  for (int i = 0; i < cmdTableLen; i++) {
    Command cmd = cmdTable[i];

    int bufferLen = (int)strlen(commandBuffer.data);
    char *argStart = strchr(commandBuffer.data, ' ');
    int commandLen = bufferLen;

    if (argStart != NULL)
      commandLen = (int)(argStart - commandBuffer.data);

    bool match = (bool)commandLen;

    if (cmd.min_abb > commandLen) {
      match = false;
    }

    for (int j = 0; j < commandLen; j++) {
      if (cmd.name[j] != commandBuffer.data[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      log("Found match '%s' %s %d", cmd.name, argStart, commandLen);
      cmd.run(argStart ? argStart + 1 : "");
      return;
    }
  }

  log("Command not found: %s", commandBuffer.data);
}
