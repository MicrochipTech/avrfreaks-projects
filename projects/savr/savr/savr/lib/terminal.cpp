/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

#include <avr/io.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdio.h>

#include <savr/terminal.h>

#define BACKSPACE_CHAR  0x08
#define DEL_CHAR        0x7F
#define CLR_CHAR        0x15
#define ESC_CHAR        0x1B

// Defined by user and linked
static PGM_P welcomeMessage;
static PGM_P promptString;

static char stringBuf[Term::LINESIZE];


///! File-scope terminal state
namespace TermState {
static uint8_t  count;
static uint8_t  length;
static char     *dest;
};


/**
 * @par Implementation notes:
 */
static void
Backspace()
{
    putchar(BACKSPACE_CHAR);
    putchar(' ');
    putchar(BACKSPACE_CHAR);
}


/**
 * @par Implementation notes:
 */
static bool
AddChar(char c)
{
    using namespace TermState;
    /* Echo back */
    putchar(c);

    /* If there's room, store the char, else, backup over it */
    if (count < (length - 1)) {
        dest[count] = c;
        count++;
        return true;
    } else {
        Backspace();
        return false;
    }
}


/**
 * @par Implementation notes:
 */
static void
ClearLine()
{
    using namespace TermState;
    while (count) {
        putchar(BACKSPACE_CHAR);
        putchar(' ');
        putchar(BACKSPACE_CHAR);
        count--;
    }
}

#if 0
static void
SetLine(char *line)
{
    using namespace TermState;

    ClearLine();
    while(*line && AddChar(*line)) {
        line++;
    }
}
#endif


/**
 * @par Implementation notes:
 */
static void
HandleEsc()
{
    using namespace TermState;

    char next = getchar();
    if(next != '[') {
        AddChar(next);
        return;
    }
    next = getchar();
    switch(next) {
    case 'A':
        break;
    case 'B':
        break;
    default:
        AddChar('[');
        AddChar(next);
        return;
    }
}



/**
 * @par Implementation Notes:
 */
void
Term::Init(PGM_P message, PGM_P prompt)
{
    welcomeMessage = message;
    promptString = prompt;

    printf_P(welcomeMessage);
}



/**
 * @par Implementation Notes:
 */
void
Term::Run(const CMD::CommandList commandList, size_t length)
{
    CMD::Init(commandList, length);
    while (1) {
        Term::GetLine(stringBuf, Term::LINESIZE);
        CMD::RunCommand(stringBuf);
    }
}




/**
 * @par Implementation Notes:
 */
void
Term::GetLine(char * string, uint8_t maxLength)
{

    char temp;

    using namespace TermState;
    count   = 0;
    dest    = string;
    length  = maxLength;


    printf_P(promptString);

    do {
        temp = getchar();
        switch (temp) {

        /* End of line found. NULL out and return. */
        case '\r':
            dest[count] = '\0';
            putchar('\n');
            //puts(dest);
            return;
            /* Not Reached */
            break;

        /* Clear line. Erase up to prompt. */
        case CLR_CHAR:
            ClearLine();
            break;

        case DEL_CHAR: /* Fall through */
        case BACKSPACE_CHAR:
            if (count) {
                Backspace();
                count--;
            }
            break;

        case ESC_CHAR:
            HandleEsc();
            break;

        /* All others, check for non-special character. */
        default:
            if (temp >= 0x20 && temp <= 0x7E) {
                AddChar(temp);
            } else {
                //printf("0x%02X ", temp);
            }

            break;
        }

    } while (1);
}

