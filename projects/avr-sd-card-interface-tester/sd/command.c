/*************************************************************//**
* \file command.c
*
* Command system
*
* This provides command functionality for a terminal interface
* A list of commands is defined by the user at compile time, which
* is passed to the initialization routine at run time.
*
* "Command line" strings are passed to the CMD_FindAndRun() routine
* which then parses the command, searches for a match, and runs the 
* callback routine.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/
#include <stdio.h>
#include <string.h>

#include "command.h"

void  CMD_FindAndRun(char *cmd, char *args);

static CommandListPtr  cmdList   = NULL;
static uint16_t        cmdLength = 0;


/**
 * Setup the command entries
 *
 * Must be called before anything else
 *
 * @param commandList the CommandList to use
 * @param length the length of the CommandList
 */
void
CMD_Init(const CommandList commandList, uint16_t length) {
  cmdList   = commandList;
  cmdLength = length;
}


/**
 * Parse a line and run the found command
 *
 * Will destroy the character string passed to it.
 *
 * @param line a character pointer to the line to parse (and destroy)
 */
void
CMD_RunCommand(char *line) {
  char *cmd;
  char *args;

  cmd  = line;
  args = line;

  // Find the space delimiter between command and arguments
  while(*args != ' ' && *args)
    args++;
  
  // If there are argument, args will be on the space delimiter
  if(*args != '\0') {
    *args = '\0';
    args++;
  }

  /* Right now both cmd and args should
    be pointing to null terminated strings */

  // Empty command? Ignore
  if(*cmd == '\0') {
    return;
  }

  // Else, try to run the command
  CMD_FindAndRun(cmd, args);
}


/**
 * Scan through the command list and run the callback, if any.
 *
 * CMD_Init should have been called first, but the length
 * would be zero if it hasn't... so I guess it's ok.
 *
 * @param cmd a pointer to the command string
 * @param args a pointer to the argument string to pass to the command callback
 */
void
CMD_FindAndRun(char *cmd, char *args) {
  unsigned int i=0;
  uint8_t found = 0;

  // Linear search, but it's not that important... right?
  for(i=0; i<cmdLength; i++) {
    if(strcmp(cmdList[i].commandName, cmd) == 0) {
      cmdList[i].callback(args);
      found = 1;
      break;
    }
  }
  
  if(!found) {
    printf("Unk cmd\n");
  }
}
