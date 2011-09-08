#ifndef _terminal_h_Included_
#define _terminal_h_Included_
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
 * @file terminal.h
 *
 * Terminal interface
 *
 * This provides a terminal-like interface to the user, relying
 * on libc and properly configured stdin and stdout.
 *
 * Supported special keys:
 *   - Backspace or Delete
 *   - Ctrl+U (clear line)
 */

#include <savr/cpp_pgmspace.h>
#include <savr/command.h>

/**
 * Terminal
 */
namespace Term {

    const uint8_t LINESIZE = 64;    ///< Line size for Run()


    /**
     * Initializes the terminal settings and prints a welcome message
     * @param message a pointer to the welcome message
     * @param prompt a pointer to the prompt string
     */ 
    void Init(PGM_P message, PGM_P prompt);


    /**
     * Run the Terminal subsystem, never returning
     *
     * This continually gets lines from standard in and runs the
     * associated command, if any.
     *
     * @param commandList the list of supported commands
     * @param length the length of the commandList
     */
    void Run(const CMD::CommandList commandList, size_t length) __attribute__ ((noreturn));


    /**
     * Gets a line of text from stdin
     *
     * Has some basic features, such as CTRL+U to clear a line
     *
     * @param string the user supplied buffer to store the input line
     * @param maxLength the number of characters to read in at max
     */
    void GetLine(char * string, uint8_t maxLength);

};
#endif
