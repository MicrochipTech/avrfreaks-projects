/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2013 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
  \file sys_port.cpp
  \brief Class Implementation for Atmel SAM-D20 Port and Pinmux functionality
*/

#include "sys_port.h"

//---------------------------------------------------------------------------
SysPort::SysPort()
{
    m_ePort = SYS_PORT_A;
    m_pstPort = GetPortPointer();
}

//---------------------------------------------------------------------------
SysPort::SysPort(PortIdentity_t ePort_)
{
    if (ePort_ > SYS_PORT_COUNT)
    {
        m_ePort = SYS_PORT_A;
    } else {
        m_ePort = ePort_;
    }

    m_pstPort = GetPortPointer();
}

//---------------------------------------------------------------------------
void SysPort::SetDir(K_UCHAR ucPinIndex_, bool bOutput_)
{
    if (bOutput_)
    {
        m_pstPort->DIRSET.reg = (1 << ucPinIndex_);
    }
    else
    {
        m_pstPort->DIRCLR.reg = (1 << ucPinIndex_);
    }
}

//---------------------------------------------------------------------------
void SysPort::SetOut(K_UCHAR ucPinIndex_, bool bLevel_)
{
    if (bLevel_)
    {
        m_pstPort->OUTSET.reg = (1 << ucPinIndex_);
    }
    else
    {
        m_pstPort->OUTCLR.reg = (1 << ucPinIndex_);
    }
}

//---------------------------------------------------------------------------
void SysPort::ToggleOut(K_UCHAR ucPinIndex_)
{
    m_pstPort->OUTTGL.reg = (1 << ucPinIndex_);
}

//---------------------------------------------------------------------------
bool SysPort::GetIn(K_UCHAR ucPinIndex_)
{
    return ((m_pstPort->IN.reg >> ucPinIndex_) & 0x1);
}

//---------------------------------------------------------------------------
void SysPort::SetPinConfig(K_UCHAR ucPinIndex_, bool bPullUp_, bool bInputEnable_, bool bMuxWithPeripheral_)
{
    K_ULONG ulReg;
    K_ULONG ulPinBit = 1 << ((K_ULONG)(ucPinIndex_ & 0x0F));

    ulReg = PORT_WRCONFIG_WRPINCFG
            | (ulPinBit << PORT_WRCONFIG_PINMASK_Pos)
            ;

    if (ucPinIndex_ >= 16)
    {
        ulReg |= PORT_WRCONFIG_HWSEL; // Accessing upper bits in the register
    }
    if (bPullUp_)
    {
        ulReg |= PORT_WRCONFIG_PULLEN;
    }
    if (bInputEnable_)
    {
        ulReg |= PORT_WRCONFIG_INEN;
    }
    if (bMuxWithPeripheral_)
    {
        ulReg |= PORT_WRCONFIG_PMUXEN;
    }

    m_pstPort->WRCONFIG.reg = ulReg;
}

//---------------------------------------------------------------------------
void SysPort::SetPortMux(K_UCHAR ucPinIndex_, PinMux_t eMuxFunction_)
{
    K_ULONG ulReg;
    K_ULONG ulPinBit = 1 << ((K_ULONG)(ucPinIndex_ & 0x0F));

    ulReg =  PORT_WRCONFIG_WRPMUX
            | (ulPinBit << PORT_WRCONFIG_PINMASK_Pos)
            | (eMuxFunction_ << PORT_WRCONFIG_PMUX_Pos)
            ;

    if (ucPinIndex_ >= 16)
    {
        ulReg |= PORT_WRCONFIG_HWSEL; // Accessing upper bits in the register
    }

    m_pstPort->WRCONFIG.reg = ulReg;
}

//---------------------------------------------------------------------------
PortGroup *SysPort::GetPortPointer()
{
    PortGroup *pstPort = 0;
    switch (m_ePort)
    {
        case SYS_PORT_A:
        {
            pstPort = &PORT->Group[0];
        }
            break;
        case SYS_PORT_B:
        {
            pstPort = &PORT->Group[1];
        }
            break;
        default:
            pstPort = &PORT->Group[0];
            break;
    }
    return pstPort;
}

//---------------------------------------------------------------------------
SysPort PortA(SYS_PORT_A);
SysPort PortB(SYS_PORT_B);

