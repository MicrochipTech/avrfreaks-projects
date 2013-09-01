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

    \file   drvRAM.cpp

    \brief  Generic RAM-buffer driver

*/

#include "kerneltypes.h"
#include "drvRAM.h"
#include "driver.h"

//---------------------------------------------------------------------------
void RAMDriver::Init()
{
    m_usAddress = 0;
}

//---------------------------------------------------------------------------
K_UCHAR RAMDriver::Open()
{
    return 0;
}

//---------------------------------------------------------------------------
K_UCHAR RAMDriver::Close()
{
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT RAMDriver::Read( K_USHORT usBytes_, K_UCHAR *pucData_ )
{
    K_USHORT usNumBytes = usBytes_;
    K_USHORT i;
    K_UCHAR *pcTemp;
    
    if ((m_usAddress + usBytes_) >= m_usSize)
    {
        usNumBytes = m_usSize - m_usAddress;
    }
    
    if (!usNumBytes)
    {
        return 0;
    }

    pcTemp = &(m_pcData[m_usAddress]);
    for (i = 0; i < usNumBytes; i++)
    {
        pucData_[i] = pcTemp[i];
    }
    
    m_usAddress += usNumBytes;
    
    return usNumBytes;
}

//---------------------------------------------------------------------------
K_USHORT RAMDriver::Write( K_USHORT usBytes_, K_UCHAR *pucData_ )
{
    K_USHORT usNumBytes = usBytes_;
    K_USHORT i;
    K_UCHAR *pcTemp;

    if ((m_usAddress + usBytes_) >= m_usSize )
    {
        usNumBytes = m_usSize - m_usAddress;
    }
    
    if (!usNumBytes)
    {
        return 0;
    }

    pcTemp = &(m_pcData[m_usAddress]);
    for (i = 0; i < usNumBytes; i++)
    {
        pcTemp[i] = pucData_[i];
    }

    m_usAddress += usNumBytes;
    
    return usNumBytes;
}

//---------------------------------------------------------------------------
K_USHORT RAMDriver::Control( K_USHORT usEvent_, void *pvIn_, K_USHORT usSizeIn_, void *pvOut_, K_USHORT usSizeOut_ )
{
    switch (usEvent_)
    {
        case EEPROM_CMD_SEEK:
        {
            if (usSizeOut_ < m_usSize)
            {
                m_usAddress = usSizeIn_;
            }
            return 1;
        }        
        case EEPROM_CMD_SET_BUFFER:
        {
            m_pcData = (K_UCHAR*)pvIn_;
            m_usSize = usSizeIn_;
        }
    }
    return 0;
}
