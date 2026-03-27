#include "command.h"
#include "buffer.h"
#include "files.h"
#include "helpers.h"
#include <string.h>

static LineBuffer commandBuffer = {"", 0};

void cmdWrite() {
  filesSaveFromBuffer("./chuj", bufferGet());
}

void cmdEdit() {
  filesLoadIntoBuffer("./chuj", bufferGet());
  log("SAVED %s", commandBuffer.data);
}

Command cmdTable[] = {
    {"edit", 2, cmdEdit},
    {"write", 1, cmdWrite}};
int cmdTableLen = 2;

LineBuffer *commandGetLineBuffer(void) {
  return &commandBuffer;
}

void commandRun(void) {
  for (int i = 0; i < cmdTableLen; i++) {
    Command cmd = cmdTable[i];

    int bufferLen = strlen(commandBuffer.data);
    // log("nie weszłeś %i %i", bufferLen, cmd.min_abb);
    bool match = (bool)bufferLen;

    //  1                   1
    if (cmd.min_abb > bufferLen) {
      match = false;
    }

    for (int j = 0; j < bufferLen; j++) {

      if (cmd.name[j] != commandBuffer.data[j]) {
        match = false;
        break;
      }
    }

    if (match) {
      cmd.run();
      log("Ran '%s'", cmd.name);
      return;
    }
  }

  log("Command not found: %s", commandBuffer.data);

  // TODO: parse and execute commandGetBuffer()
}