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

    \file   drvEEPROM.cpp

    \brief  Atmega eeprom driver

*/
#include "kerneltypes.h"

#include "drvEEPROM.h"
#include "driver.h"

#include <avr/io.h>
#include <avr/eeprom.h>

//---------------------------------------------------------------------------

#define EEPROM_SIZE        (4096)

//---------------------------------------------------------------------------
void ATMegaEEPROM::Init()
{ 
    m_usAddress = 0;
}

//---------------------------------------------------------------------------
K_UCHAR ATMegaEEPROM::Open()
{ 
    return 0; 
}

//---------------------------------------------------------------------------
K_UCHAR ATMegaEEPROM::Close()
{ 
    return 0; 
}
    
//---------------------------------------------------------------------------
K_USHORT ATMegaEEPROM::Read( K_USHORT usBytes_, K_UCHAR *pucData_ )
{
    K_USHORT usNumBytes = usBytes_;

    if ((m_usAddress + usBytes_) >= EEPROM_SIZE)
    {
        usNumBytes = EEPROM_SIZE - m_usAddress;
    }
        
    if (!usNumBytes)
    {
        return 0;
    }

    eeprom_read_block((void*)pucData_, (const void*)m_usAddress, (size_t)usNumBytes);
    m_usAddress += usNumBytes;
        
    return usNumBytes;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaEEPROM::Write( K_USHORT usBytes_, K_UCHAR *pucData_ )
{
    K_USHORT usNumBytes = usBytes_;

    if ((m_usAddress + usBytes_) >= EEPROM_SIZE)
    {
        usNumBytes = EEPROM_SIZE - m_usAddress;
    }
    
    if (!usNumBytes)
    {
        return 0;
    }

    eeprom_write_block((void*)pucData_, (void*)m_usAddress, usNumBytes);
    m_usAddress += usNumBytes;
    
    return usNumBytes;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaEEPROM::Control( K_USHORT usEvent_, void *pvIn_, K_USHORT usSizeIn_, void *pvOut_, K_USHORT usSizeOut_ )
{
    switch (usEvent_)
    {    
        case EEPROM_CMD_SEEK:
        {
            if (usSizeOut_ < EEPROM_SIZE)
            {
                m_usAddress = usSizeIn_;
            }
            return 1;
        }
    }
    return 0;
}
