#ifndef _COMMAND_H_
#define _COMMAND_H_ 1

/*************************************************************//**
* \file command.h
*
* Command system header
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/
#include <inttypes.h>
#include <stdlib.h>


//! Name and callback for a command
typedef struct {
  //! Name of the command
  const char *commandName;
  //! Callback function
  //! Takes a character pointer containing arguments entered after the command
  uint8_t   (*callback)(char * args);
}CommandDef;

//! List of command definitions for const initialization
typedef const CommandDef  CommandList[];
//! List of command definitions for argument passing
typedef const CommandDef *CommandListPtr;


void    CMD_Init(const CommandList commandList, uint16_t length);
void    CMD_RunCommand(char * line);

#endif
