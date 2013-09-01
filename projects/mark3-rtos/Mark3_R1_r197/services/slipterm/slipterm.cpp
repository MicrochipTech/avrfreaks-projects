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
    \file slipterm.cpp
    
    \brief Serial debug interface using SLIP protocol, and FunkenSlip 
           multiplexing.
*/

#include "kerneltypes.h"
#include "slip.h"
#include "slipterm.h"

//---------------------------------------------------------------------------
void SlipTerm::Init()
{
    m_clSlip.SetDriver( DriverList::FindByPath("/dev/tty" ) );            
    m_ucVerbosity = SEVERITY_DEBUG;
}

//---------------------------------------------------------------------------
K_USHORT SlipTerm::StrLen( const char *szLine_ )
{
    K_USHORT i=0;
    while (szLine_[i] != 0 ) 
    {
        i++;
    }
    return i;
}

//---------------------------------------------------------------------------
void SlipTerm::PrintLn( const char *szLine_ )
{
    SlipDataVector astData[2];
    astData[0].pucData = (K_UCHAR*)szLine_;
    astData[0].ucSize = StrLen(szLine_);
    astData[1].pucData = (K_UCHAR*)"\r\n";
    astData[1].ucSize = 2;
    
    m_clSlip.WriteVector(SLIP_CHANNEL_TERMINAL, astData, 2);
}

//---------------------------------------------------------------------------
void SlipTerm::PrintLn( K_UCHAR ucSeverity_, const char *szLine_ )
{
    if (ucSeverity_ <= m_ucVerbosity)
    {
        PrintLn( szLine_ );
    }
}
