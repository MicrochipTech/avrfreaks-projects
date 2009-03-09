/*************************************************************//**
* \file terminal.c
*
* Terminal interface
*
* This provides a terminal-like interface to the user, relying
* on libc and properly configured stdin and stdout.
*
* Supported special keys:
*   - Backspace or Delete
*   - Ctrl+U (clear line)
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/
#include <avr/io.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

#include "terminal.h"

#define BACKSPACE_CHAR  0x08
#define DEL_CHAR        0x7F
#define CLR_CHAR        0x15


static const char * welcomeMessage;
static const char * promptString;

static char stringBuf[TERM_LINESIZE];

/**
 * Run the terminal
 *
 * Reads in lines from stdin and attemps to run given commands
 *
 * This function never returns
 */
void
TERM_Run(void) {
  while(1) {
    TERM_GetLine(stringBuf, TERM_LINESIZE);
    CMD_RunCommand(stringBuf);
  }
}

/**
 * Gets a line of text from stdin
 *
 * Has some basic features, such as CTRL+U to clear a line
 *
 * @param string the user supplied buffer to store the input line
 * @param length the number of characters to read in at max
 */
void
TERM_GetLine(char * string, uint8_t length) {
  uint8_t i = 0;
  char temp;

  printf("%s", (char *)promptString);

  do{
    temp = getchar();
    switch(temp) {

      /* End of line found. NULL out and return. */
      case '\r':
        string[i] = '\0';
        putchar('\n');
        //puts(string);
        return;
        /* Not Reached */
        break;

      /* Clear line. Erase up to prompt. */
      case CLR_CHAR:
        while(i) {
          putchar(BACKSPACE_CHAR);
          putchar(' ');
          putchar(BACKSPACE_CHAR);
          i--;
        }
        break;

      case DEL_CHAR: /* Fall through */
      case BACKSPACE_CHAR:
        if(i) {
          putchar(BACKSPACE_CHAR);
          putchar(' ');
          putchar(BACKSPACE_CHAR);
          i--;
        }
        break;

      /* All others, check for non-special character. */
      default:
        if(temp >= 0x20 && temp <= 0x7E) {
          /* Echo back */
          fputc(temp, stdout);

          /* If there's room, store the char, else, backup over it */
          if(i<(length-1)) {
            string[i] = temp;
            i++;
          }else{
            putchar(BACKSPACE_CHAR);
            putchar(' ');
            putchar(BACKSPACE_CHAR);
          }
        }else{
          printf("%02X", temp);
        }

        break;
    }

  }while(1);
}

/**
 * Initializes the terminal settings and prints a welcome message
 *
 * @param commandList the list of supported commands
 * @param length the length of the commandList
 * @param message a pointer to the welcome message
 * @param prompt a pointer to the prompt string
 */
void
TERM_Init(const CommandList commandList, uint16_t length,
          const char * message, const char * prompt) {

  welcomeMessage = message;
  promptString = prompt;

  CMD_Init(commandList, length);

  printf("%s\n", (char *)welcomeMessage);
}
