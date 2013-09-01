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

    \brief  SAM D20 serial port driver
*/

#include "drvUART.h"

//---------------------------------------------------------------------------
void D20_UART::Init(void)
{    
    m_clUART.SetInterface(SERCOM_IF_3);
    m_clUART.SetTxPad(SERCOM_PAD_2);
    m_clUART.SetRxPad(SERCOM_PAD_3);
    m_clUART.SetMux(SERCOM_MUX_C);

    m_clUART.SetParity(false);
    m_clUART.SetStopBits(1);
    m_clUART.SetBaud(57600);
}

//---------------------------------------------------------------------------
K_UCHAR D20_UART::Open()
{
    m_clUART.Open();
    return 0;
}

//---------------------------------------------------------------------------
K_UCHAR D20_UART::Close(void)
{
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT D20_UART::Control( K_USHORT usCmdId_, void *pvIn_, K_USHORT usSizeIn_, void *pvOut_, K_USHORT usSizeOut_)
{
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT D20_UART::Read( K_USHORT usSizeIn_, K_UCHAR *pvData_ )
{
    return usSizeIn_;
}

//---------------------------------------------------------------------------
K_USHORT D20_UART::Write(K_USHORT usSizeOut_, K_UCHAR *pvData_)
{
    for (K_USHORT i = 0; i < usSizeOut_; i++)
    {
        m_clUART.Write(pvData_[i]);
    }
	return usSizeOut_;
}
