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
 * @file command.h
 *
 * Command system
 *
 * This provides command functionality for a terminal interface
 * A list of commands is defined by the user at compile time, which
 * is passed to the initialization routine at run time.
 *
 * "Command line" strings are passed to the CMD::FindAndRun() routine
 * which then parses the command, searches for a match, and runs the
 * callback routine.
 */

#ifndef _command_h_Included_
#define _command_h_Included_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

namespace CMD {

    //! Takes a character pointer containing arguments entered after the command
    typedef uint8_t (*CommandCallback)(char *args);

    //! Name and callback for a command
    typedef struct {
        //! Name of the command
        const char *commandName;
    
        //! Callback function
        CMD::CommandCallback callback;
    
        //! Brief (very brief) description
        const char *helpText;
    } CommandDef;


    typedef const CommandDef CommandList[];     ///< List of command definitions for const initialization
    typedef const CommandDef *CommandListPtr;   ///< List of command definitions for argument passing

    /**
     * Initialize the Command subsystem
     *
     * @param commandList   List of supported commands
     * @param length        Number of commands in list
     */
    void Init(const CommandList commandList, size_t length);


    /**
     * Parse a given line and run a command, if found
     *
     * @param line  The null-terminated line to parse
     *
     * If the command is not found, a small help text is displayed
     */
    void RunCommand(char *line);

};

#endif

