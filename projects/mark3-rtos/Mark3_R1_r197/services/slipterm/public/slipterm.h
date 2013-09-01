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
    \file slipterm.h
    
    \brief Serial debug interface using SLIP serial, and Funkenslip serial
           port multiplexing
*/

#include "kerneltypes.h"
#include "driver.h"
#include "slip.h"

//---------------------------------------------------------------------------
#define SEVERITY_DEBUG                4
#define SEVERITY_INFO                3
#define SEVERITY_WARN                2
#define SEVERITY_CRITICAL            1
#define SEVERITY_CATASTROPHIC        0

//---------------------------------------------------------------------------
#ifndef __SLIPTERM_H__
#define __SLIPTERM_H__

/*!
    Class implementing a simple debug terminal interface.  This is
    useful for printf style debugging.
*/
class SlipTerm
{
public:
    /*!
        \fn void Init();
        
        Initialize the terminal by opening a handle to the serial interface
        attached at /dev/tty.  Must be called prior to using the print
        functionality.
    */
    void Init();
    
    /*!
        \fn void PrintLn( const char *szLine_ )
        
        Print a string of text to the SLIP interface, multiplexed using the
        FunkenSlip terminal channel.
        
        \param szLine_ String to print
    */
    void PrintLn( const char *szLine_ );

    /*!
        \fn void PrintLn( K_UCHAR ucSeverity_, const char *szLine_ )
        
        Print a string of text to the SLIP interface, but only if the 
        current logging verbosity level is greater than or equal to the 
        specified message severity
        
        \param ucSeverity_ Message severity level, 0 = highest severity
        \param szLine_ String to print
    */
    void PrintLn( K_UCHAR ucSeverity_, const char *szLine_ );
    
    /*!        
        \fn void SetVerbosity( K_UCHAR ucLevel_ )
        
        Set the logging verbosity level - the minimum severity level that
        will be printed to the terminal.  The higher the number, the 
        more chatty the output.
    */
    void SetVerbosity( K_UCHAR ucLevel_ ) { m_ucVerbosity = ucLevel_; }
private:
    /*!
        \fn K_USHORT StrLen( const char *szString_ )
        
        Quick 'n' dirty StrLen functionality used for printing the string.
        
        \return Length of the string (in bytes)
    */
    K_USHORT StrLen( const char *szString_ );
    
    K_UCHAR m_ucVerbosity;     //!< Verbosity level.  Messages with a severity 
                            //!< level greater than this Are not displayed.
    
    Slip m_clSlip;        //!< Slip object that this module interfaces with
};

#endif
