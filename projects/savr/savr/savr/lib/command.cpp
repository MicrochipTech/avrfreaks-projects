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

/**
 * @file command.cpp
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <savr/cpp_pgmspace.h>
#include <savr/command.h>

static void FindAndRun(char *cmd, char *args);
static void Help(void);

static CMD::CommandListPtr cmdList = NULL;

static size_t   cmdLength;
static size_t   commandColWidth;
static char     formatter[32];


/**
 * @brief Setup the command entries
 *
 * Must be called before anything else
 *
 * @param commandList the CommandList to use
 * @param length the length of the CommandList
 */
void
CMD::Init(const CMD::CommandList commandList, size_t length)
{
    size_t temp = 0;

    cmdList = commandList;
    cmdLength = length;

    commandColWidth = 0;

    for (size_t i = 0; i < length; ++i) {
        temp = strlen(commandList[i].commandName);
        if (temp > commandColWidth)
            commandColWidth = temp;
    }

    // For pretty help formatting
    sprintf_P(formatter, PSTR("  %%-%ds  %%s\n"), commandColWidth);
}


/**
 * @brief Parse a line and run the found command
 *
 * Will destroy the character string passed to it.
 *
 * @param line a character pointer to the line to parse (and destroy)
 */
void
CMD::RunCommand(char *line)
{
    char *cmd;
    char *args;

    cmd = line;
    args = line;

    // Find the space delimiter between command and arguments
    while (*args != ' ' && *args)
        args++;

    // If there are arguments, args will be on the space delimiter
    if (*args != '\0') {
        *args = '\0';
        args++;
    }

    /* Right now both cmd and args should
     be pointing to null terminated strings */

    // Empty command? Ignore
    if (*cmd == '\0') {
        return;
    }

    // Else, try to run the command
    FindAndRun(cmd, args);
}


/**
 * @brief Scan through the command list and run the callback, if any.
 *
 * CMD::Init should have been called first, but the length
 * would be zero if it hasn't... so I guess it's ok.
 *
 * @param cmd a pointer to the command string
 * @param args a pointer to the argument string to pass to the command callback
 */
void
FindAndRun(char *cmd, char *args)
{
    size_t i = 0;
    uint8_t found = 0;
    uint8_t ret = 0;

    // Linear search, but it's not that important... right?
    for (i = 0; i < cmdLength; i++) {
        if (strcmp(cmdList[i].commandName, cmd) == 0) {
            ret = cmdList[i].callback(args);
            found = 1;
            break;
        }
    }

    if (!found) {
        if (strcmp_P(cmd, PSTR("help")) == 0) {
            Help();
        } else {
            printf_P(PSTR("Unk cmd. Try 'help'.\n"));
        }
    }
}


/**
 * @brief Prints out a list of supported commands.
 */
void
Help(void)
{
    size_t i;

    // List out all available commands
    printf_P(PSTR("Available commands:\n"));
    for (i = 0; i < cmdLength; i++) {
        if (cmdList[i].helpText == NULL) {
            printf_P(PSTR("  %s\n"), cmdList[i].commandName);
        } else {
            printf(formatter, cmdList[i].commandName, cmdList[i].helpText);
        }
    }
}

