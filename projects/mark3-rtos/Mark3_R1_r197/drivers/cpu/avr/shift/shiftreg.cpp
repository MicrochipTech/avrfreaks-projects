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
    \file shiftreg.cpp
    \brief Shift register functionality to allow for efficient I/O expansion
*/
#include "kerneltypes.h"
#include "driver.h"
#include "shiftreg.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//---------------------------------------------------------------------------
K_UCHAR ShiftDriver::Open()
{
    *m_pucEnablePort |= m_ucEnableBit;
    return 0;
}

//---------------------------------------------------------------------------
K_UCHAR ShiftDriver::Close()
{
    *m_pucEnablePort &= ~m_ucEnableBit;
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT ShiftDriver::Write( K_USHORT usBytes_, 
                              K_UCHAR *pucData_)
{
    K_USHORT i;
    
    // Only update the strobe after clocking out all of the required bytes.
    // This allows the user to make the scan chain as long as possible without
    // having to modify the driver.
    *m_pucStrobePort &= ~m_ucStrobeBit;    
    for (i = 0; i < usBytes_; i++)
    {
        WriteByte(pucData_[i]);
    }    
    *m_pucStrobePort |= m_ucStrobeBit;
    return usBytes_;
    
}                              
//---------------------------------------------------------------------------
K_USHORT ShiftDriver::Control( K_USHORT usEvent_, 
                                void *pvDataIn_, 
                                K_USHORT usSizeIn_, 
                                void *pvDataOut_, 
                                K_USHORT usSizeOut_ )                                
{
    switch (usEvent_)
    {
        case SHIFT_SET_CLOCK:
            m_pucClockPort = (volatile K_UCHAR*)pvDataIn_;
            m_ucClockBit = (K_UCHAR)usSizeIn_;
        break;
        case SHIFT_SET_DATA:
            m_pucDataPort = (volatile K_UCHAR*)pvDataIn_;
            m_ucDataBit = (K_UCHAR)usSizeIn_;
        break;
        case SHIFT_SET_STROBE:
            m_pucStrobePort = (volatile K_UCHAR*)pvDataIn_;
            m_ucStrobeBit = (K_UCHAR)usSizeIn_;
        break;
        case SHIFT_SET_ENABLE:
            m_pucEnablePort = (volatile K_UCHAR*)pvDataIn_;
            m_ucEnableBit = (K_UCHAR)usSizeIn_;
        break;
        default:
            break;
    }
    return 0;
}
//---------------------------------------------------------------------------
void ShiftDriver::SetClock( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ )
{
    Control( SHIFT_SET_CLOCK, (void*)pucPort_, (K_USHORT)ucBit_, 0, 0 );
}
//---------------------------------------------------------------------------
void ShiftDriver::SetData( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ )
{
    Control( SHIFT_SET_DATA, (void*)pucPort_, (K_USHORT)ucBit_, 0, 0 );
}
//---------------------------------------------------------------------------
void ShiftDriver::SetStrobe( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ )
{
    Control( SHIFT_SET_STROBE, (void*)pucPort_, (K_USHORT)ucBit_, 0, 0 );
}
//---------------------------------------------------------------------------
void ShiftDriver::SetEnable( volatile K_UCHAR *pucPort_, K_UCHAR ucBit_ )
{
    Control( SHIFT_SET_ENABLE, (void*)pucPort_, (K_USHORT)ucBit_, 0, 0 );
}
//---------------------------------------------------------------------------
void ShiftDriver::WriteByte( K_UCHAR ucByte_ )
{
    K_UCHAR ucMask = 0x80;
    
    // Clock data out, MSB first, data latched on the RISING clock edge.
    while (ucMask)
    {
        *m_pucClockPort &= ~m_ucClockBit;
        if (ucByte_ & ucMask)
        {
            *m_pucDataPort |= m_ucDataBit;
        }
        else
        {
            *m_pucDataPort &= ~m_ucDataBit;
        }
        ucMask >>= 1;
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        *m_pucClockPort |= m_ucClockBit;
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
        asm volatile("nop");
    }
}
