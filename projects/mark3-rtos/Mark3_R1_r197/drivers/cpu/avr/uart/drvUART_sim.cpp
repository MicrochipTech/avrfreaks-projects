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

    \file   drvUART.cpp

    \brief  Atmega328p serial port driver
*/

#include "kerneltypes.h"
#include "drvUART.h"
#include "driver.h"
#include "thread.h"
#include "threadport.h"
#include "kerneltimer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------------------------------------------------------------------
static ATMegaUART *pclActive;	// Pointer to the active object

//---------------------------------------------------------------------------
void ATMegaUART::SetBaud(void)
{    
}

//---------------------------------------------------------------------------
void ATMegaUART::Init(void)
{    
}

//---------------------------------------------------------------------------
K_UCHAR ATMegaUART::Open()
{
	return 0;
}

//---------------------------------------------------------------------------
K_UCHAR ATMegaUART::Close(void)
{
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaUART::Control( K_USHORT usCmdId_, void *pvIn_, K_USHORT usSizeIn_, void *pvOut_, K_USHORT usSizeOut_)
{
    switch ((CMD_UART)usCmdId_)
    {
        case CMD_SET_BAUDRATE:
        {
        }
            break;
        case CMD_SET_BUFFERS:
        {
        }            
            break;		
        case CMD_SET_RX_ESCAPE:
        {
        }
            break;
        case CMD_SET_RX_CALLBACK:
        {
        }
            break;
        case CMD_SET_RX_ECHO:
        {
        }
            break;
		case CMD_SET_RX_ENABLE:
		{
		}
			break;
		case CMD_SET_RX_DISABLE:
		{
		}
			break;						
        default:
            break;
    }
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaUART::Read( K_USHORT usSizeIn_, K_UCHAR *pvData_ )
{
    return fread( pvData_, 1, usSizeIn_, stderr );
}

//---------------------------------------------------------------------------
K_USHORT ATMegaUART::Write(K_USHORT usSizeOut_, K_UCHAR *pvData_)
{
    return fwrite( pvData_, 1, usSizeOut_, stderr );
}

//---------------------------------------------------------------------------
void ATMegaUART::StartTx(void)
{
    // stub
}

//---------------------------------------------------------------------------
void ATMegaUART::RxISR()
{
    // stub
}

//---------------------------------------------------------------------------
void ATMegaUART::TxISR()
{
    // Stub
}
