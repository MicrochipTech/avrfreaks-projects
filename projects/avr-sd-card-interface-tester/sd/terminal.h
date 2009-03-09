#ifndef _TERMINAL_H_
#define _TERMINAL_H_ 1

/*************************************************************//**
* \file terminal.h
*
* Terminal interface header
*
* This provides a terminal-like interface to the user, relying
* on libc and properly configured stdin and stdout.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/

#include "command.h"


//! Max number of characters for a line input
#define TERM_LINESIZE 64

void  TERM_Init(const CommandList, uint16_t, const char *, const char*);
void  TERM_GetLine(char * string, uint8_t length);
void  TERM_Run(void);

#endif
