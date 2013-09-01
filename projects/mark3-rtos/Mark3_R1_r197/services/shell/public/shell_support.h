/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   shell_support.h

    \brief  Support functions & data structures useful in implementing a
            shell.

*/

#ifndef __SHELL_SUPPORT_H__
#define __SHELL_SUPPORT_H__

//---------------------------------------------------------------------------
#include "kerneltypes.h"
#include "memutil.h"

//---------------------------------------------------------------------------
#ifndef MIN
    /*!
        Utility macro used to return the lesser of two values/objects
    */
    #define MIN(x,y)        ( ( (x) < (y) ) ? (x) : (y) )
#endif
#ifndef MAX
    /*!
        Utility macro used to return the greater of two values/objects
    */
    #define MAX(x,y)        ( ( (x) > (y) ) ? (x) : (y) )
#endif

//---------------------------------------------------------------------------
/*!
    Structure used to represent a command-line option with its arguments.
    An option is defined as any token beginning with a "-" value.  The tokens
    arguments are subsequent tokens that do not begin with "-".

    Where no "-" values are specified, each token becomes its own option.

    i.e. given the following command-line

        mycmd -opt1 a b c -opt2 d e f -opt 3

    The possible Option_t structures would be:

        pstStart => Array containing tokens for -opt1, a, b, c
        ucCount  => 4 (4 tokens, including the option token, "-opt1")

        pstStart => Array containing tokens for -opt2, d, e, f
        ucCount  => 4 (4 tokens, including the option token, "-opt2")

        pstStart => Array containing tokens for -opt, 3
        ucCount  => 2 (2 tokens, including the option token, "-opt3")

    in the case of:

        mycmd a b c

    Possible token values would be:

        pstStart => Array containing tokens for a
        ucCount  => 1

        pstStart => Array containing tokens for b
        ucCount  => 1

        pstStart => Array containing tokens for c
        ucCount  => 1

*/
typedef struct
{
    Token_t *pstStart;  //!< Pointer to the beginning of a token array contain the option and its arguments
    K_UCHAR ucCount;    //!< Number of tokens in the token array
} Option_t;

//---------------------------------------------------------------------------
/*!
    Structure containing multiple representations for command-line data
*/
typedef struct
{
    Token_t *pastTokenList;      //!< Pointer to the list of tokens in the commandline
    K_UCHAR ucTokenCount;        //!< Count of tokens in the token list

    Token_t *pstCommand;        //!< Pointer to the token corresponding to the shell command

    Option_t astOptions[12];    //!< Option strucure array built from the token list
    K_UCHAR ucNumOptions;       //!< Number of options parsed from the token list
} CommandLine_t;

//---------------------------------------------------------------------------
/*!
 * Function pointer type used to represent shell commands, as implemented
 * by users of this infrastructure.  Commands return a signed 8-bit result,
 * and take a command-line argument structure as the first and only argument.
 */
typedef K_CHAR (*fp_internal_command)( CommandLine_t *pstCommandLine_ );

//---------------------------------------------------------------------------
/*!
    Data structure defining a lookup table correlating a command name to its
    handler function.
*/
typedef struct
{
    const K_CHAR *szCommand;        //!< Command name
    fp_internal_command pfHandler;  //!< Command handler function
} ShellCommand_t;

//---------------------------------------------------------------------------
/*!
 * \brief The ShellSupport class features utility functions which handle
 *        token processing, option/parameter lookup, and functions making it
 *        generally trivial to implement a lightweight custom shell.
 */
class ShellSupport
{
public:

    //---------------------------------------------------------------------------
    /*!
     * \brief RunCommand    Given a command-line, attempts to run the corresponding
     *                      shell command based where it exists within the supplied
     *                      ShellCommand_t array.
     * \param pstCommand_   Pointer to the command-line to execute
     * \param pstCommands_  Pointer to an array of shell commands to execute against
     * \return 1 on success, 0 on error (command not found)
     */
    static K_CHAR RunCommand( CommandLine_t *pstCommand_, const ShellCommand_t *pastShellCommands_ );

    //---------------------------------------------------------------------------
    /*!
     * \brief UnescapeToken Convert a token which has special parsing characters in it
     *                      to a "flattened" string, where all unescaped double quotes
     *                      and escaped tab, newline, space, etc. characters are converted
     *                      into their ascii-code equivalents.
     *
     * \param pstToken_     Pointer to the source token to convert
     * \param szDest_       Pointer to a destination string which will contain the
     *                      parsed result string
     */
    static void UnescapeToken( Token_t *pstToken_, K_CHAR *szDest_ );

    //---------------------------------------------------------------------------
    /*!
     * \brief CheckForOption Check to see whether or not a specific option has been
     *                       set within the commandline arguments.
     *
     * \param pstCommand_    Pointer to the commandline object containing the options
     *
     * \param szOption_      0-terminated string corresponding to the command-line
     *                       option.
     *
     * \return               Pointer to the command line option on match, or 0 on
     *                       faiulre.
     */
    static Option_t *CheckForOption( CommandLine_t *pstCommand_, const K_CHAR *szOption_ );

    //---------------------------------------------------------------------------
    /*!
     * \brief TokensToCommandLine   Convert an array of tokens to a commandline object.
     *                              This operation is non-destructive to the source token
     *                              array.
     * \param pastTokens_           Pointer to the token array to process
     * \param ucTokens_             Number of tokens in the token array
     * \param pstCommand_           Pointer to the CommandLine_t object which will represent
     *                              the shell command and its arguments.
     * \return  Number of options processed
     */
    static K_CHAR TokensToCommandLine(Token_t *pastTokens_, K_UCHAR ucTokens_, CommandLine_t *pstCommand_);

};



#endif // SHELL_SUPPORT_H
