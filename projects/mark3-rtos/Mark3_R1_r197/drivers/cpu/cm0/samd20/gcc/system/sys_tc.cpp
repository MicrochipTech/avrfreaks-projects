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
  \file  sys_tc.cpp
  \brief Atmel SAM D20 Timer Capture Class implementation
 */

#include "sys_tc.h"

//---------------------------------------------------------------------------
void SysTC::SetInterface( TCXBit_t eBit_ )
{
    m_uTCXBit = eBit_;
    m_pstTC = GetInterface();
}

//---------------------------------------------------------------------------
void SysTC::SetCaptureVal( K_UCHAR ucIndex_, K_ULONG ulCaptureVal_ )
{
    switch (m_eMode)
    {
        case TC_MODE_16BIT:
            m_pstTC->COUNT16.CC[ucIndex_].reg = (K_USHORT)ulCaptureVal_;
            break;
        case TC_MODE_8BIT:
            m_pstTC->COUNT8.CC[ucIndex_].reg = (K_UCHAR)ulCaptureVal_;
            break;
        case TC_MODE_32BIT:
            m_pstTC->COUNT32.CC[ucIndex_].reg = (K_ULONG)ulCaptureVal_;
            break;
        default:
            break;
    }
}

//---------------------------------------------------------------------------
void SysTC::EnableCapture( K_UCHAR ucIndex_, bool bEnable_)
{
    switch (ucIndex_)
    {
        case 0:
            m_bCapture0 = bEnable_;
        case 1:
            // m_bCapture1 = bEnable_;
        default:
            break;
    }
}

//---------------------------------------------------------------------------
K_ULONG SysTC::GetCount( void )
{
    //!! Will need to check out the example code for define names...
    m_pstTC = GetInterface();

    // If we're not continuously reading the count register...
    if(!((m_eSyncReg == TC_READSYNC_COUNT) && m_bSyncContinuous))
    {
        // Request a single read of the count register
        m_pstTC->COUNT16.READREQ.reg = TC_READREQ_RREQ |
                                        (0x10 << TC_READREQ_ADDR_Pos);
    }

    // Wait for clock domain synchronization
    WriteSync();
    switch (m_eMode)
    {
        case TC_MODE_16BIT:
            return (K_ULONG)m_pstTC->COUNT16.COUNT.reg;
            break;
        case TC_MODE_8BIT:
            return (K_ULONG)m_pstTC->COUNT8.COUNT.reg;
            break;
        case TC_MODE_32BIT:
            return m_pstTC->COUNT32.COUNT.reg;
            break;
        default:
            break;
    }

    return 0;
}

//---------------------------------------------------------------------------
void SysTC::Start( void )
{
    SetupClocks();
    SetupRegisters();

    // And Enable..
    WriteSync();
    m_pstTC->COUNT32.CTRLA.reg |= TC_CTRLA_ENABLE;
}

//---------------------------------------------------------------------------
void SysTC::Stop( void )
{
    WriteSync();
    m_pstTC->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
}

//---------------------------------------------------------------------------
void SysTC::EnableInterrupt( TCInt_t eInt_ )
{
    switch(eInt_)
    {
    case TC_INT_ERR:
        m_pstTC->COUNT32.INTENSET.reg = TC_INTENSET_ERR;
        break;
    case TC_INT_MC0:
        m_pstTC->COUNT32.INTENSET.reg = TC_INTENSET_MC(0);
        break;
    case TC_INT_MC1:
        m_pstTC->COUNT32.INTENSET.reg = TC_INTENSET_MC(1);
        break;
    case TC_INT_OVF:
        m_pstTC->COUNT32.INTENSET.reg = TC_INTENSET_OVF;
        break;
    case TC_INT_SYNCRDY:
        m_pstTC->COUNT32.INTENSET.reg = TC_INTENSET_SYNCRDY;
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------
void SysTC::DisableInterrupt( TCInt_t eInt_ )
{
    switch(eInt_)
    {
    case TC_INT_ERR:
        m_pstTC->COUNT32.INTFLAG.reg &= ~TC_INTENSET_ERR;
        break;
    case TC_INT_MC0:
        m_pstTC->COUNT32.INTFLAG.reg &= ~TC_INTENSET_MC(0);
        break;
    case TC_INT_MC1:
        m_pstTC->COUNT32.INTFLAG.reg &= ~TC_INTENSET_MC(1);
        break;
    case TC_INT_OVF:
        m_pstTC->COUNT32.INTFLAG.reg &= ~TC_INTENSET_OVF;
        break;
    case TC_INT_SYNCRDY:
        m_pstTC->COUNT32.INTFLAG.reg &= ~TC_INTENSET_SYNCRDY;
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------
void SysTC::ClearInterrupt( TCInt_t eInt_)
{
    switch(eInt_)
    {
        case TC_INT_ERR:
        m_pstTC->COUNT32.INTENCLR.reg = TC_INTENSET_ERR;
        break;
        case TC_INT_MC0:
        m_pstTC->COUNT32.INTENCLR.reg = TC_INTENSET_MC(0);
        break;
        case TC_INT_MC1:
        m_pstTC->COUNT32.INTENCLR.reg = TC_INTENSET_MC(1);
        break;
        case TC_INT_OVF:
        m_pstTC->COUNT32.INTENCLR.reg = TC_INTENSET_OVF;
        break;
        case TC_INT_SYNCRDY:
        m_pstTC->COUNT32.INTENCLR.reg = TC_INTENSET_SYNCRDY;
        break;
        default:
            break;
    }
}

//---------------------------------------------------------------------------
K_UCHAR SysTC::GetInterfaceIndex()
{
    K_UCHAR ucInt;
    switch (m_eMode)
    {
        case TC_MODE_8BIT:
            ucInt = (K_UCHAR)m_uTCXBit.e8Bit;
            break;
        case TC_MODE_16BIT:
            ucInt = ((K_UCHAR)m_uTCXBit.e16Bit) << 1;
            break;
        case TC_MODE_32BIT:
            ucInt = ((K_UCHAR)m_uTCXBit.e32Bit) << 2;
            break;
        default:
            ucInt = (K_UCHAR)m_uTCXBit.e8Bit;
            break;
    }
    return ucInt;
}

//---------------------------------------------------------------------------
Tc *SysTC::GetInterface()
{
    K_UCHAR ucInt = GetInterfaceIndex();
    Tc *pstTC = 0;
    switch (ucInt)
    {
    case 0:
        pstTC = TC0;
        break;
    case 1:
        pstTC = TC1;
        break;
    case 2:
        pstTC = TC2;
        break;
    case 3:
        pstTC = TC3;
        break;
    case 4:
        pstTC = TC4;
        break;
    case 5:
        pstTC = TC5;
        break;
    case 6:
        pstTC = TC6;
        break;
    case 7:
        pstTC = TC7;
        break;
    default:
        break;
    }
    return pstTC;
}

//---------------------------------------------------------------------------
void SysTC::SetupClocks()
{
    //Enable the TCx Clock from the power-management module
    K_UCHAR ucInt = GetInterfaceIndex();
    SysClock clClock;

    PM->APBCMASK.reg |= 1 << (PM_APBCMASK_TC0_Pos + ucInt);

    // Setup the clock/generator link attached to this peripheral
    ucInt >>= 1;    // Clocks are grouped in pairs of TCx
    clClock.SetClockID((Clock_t)(ucInt + (K_UCHAR)CLK_TC0_1));

    clClock.SetGenerator(m_eClockGen);
    clClock.CommitConfig();
    clClock.Enable(true);
}

//---------------------------------------------------------------------------
void SysTC::SetupRegisters()
{
    K_ULONG ulCtrlA = 0;
    K_ULONG ulCtrlB = 0;
    K_ULONG ulCtrlC = 0;
    K_ULONG ulReadReq = 0;
    K_ULONG ulEventCtrl = 0;

    m_pstTC = GetInterface();

    ulCtrlA = (((K_ULONG)m_eWaveformMode) << TC_CTRLA_WAVEGEN_Pos)
            | (((K_ULONG)m_eMode) << TC_CTRLA_MODE_Pos)
            | (((K_ULONG)m_ePrescalar) << TC_CTRLA_PRESCALER_Pos)
            | (((K_ULONG)m_ePresync) << TC_CTRLA_PRESCSYNC_Pos);

    ulCtrlB = (((K_ULONG)m_bOneShot) << TC_CTRLBSET_ONESHOT_Pos)
            | (((K_ULONG)m_bDirection) << TC_CTRLBSET_DIR_Pos);

    ulCtrlC = (((K_ULONG)m_bCapture0) << TC_CTRLC_CPTEN_Pos)
            | (((K_ULONG)m_bInvert0) << TC_CTRLC_INVEN_Pos);

    ulEventCtrl = (((K_ULONG)m_bMatchCompare0) << TC_EVCTRL_MCEO_Pos)
                | (((K_ULONG)m_bOverflowEnable) << TC_EVCTRL_OVFEO_Pos)
                | (((K_ULONG)m_bTCEventEnable) << TC_EVCTRL_TCEI_Pos)
                | (((K_ULONG)m_bTCEventInvert) << TC_EVCTRL_TCINV_Pos);

    if (m_bSyncContinuous)
    {
        ulReadReq |= (((K_ULONG)m_bSyncContinuous) << TC_READREQ_RCONT_Pos);
        if (m_eSyncReg == TC_READSYNC_CC0)
        {
            ulReadReq |= (0x18 << TC_READREQ_ADDR_Pos);
        }
        else if (m_eSyncReg == TC_READSYNC_CC1)
        {
            if (m_eMode == TC_MODE_8BIT)
            {
                ulReadReq |= (0x19 << TC_READREQ_ADDR_Pos);
            }
            else if (m_eMode == TC_MODE_16BIT)
            {
                ulReadReq |= (0x1A << TC_READREQ_ADDR_Pos);
            }
            else
            {
                ulReadReq |= (0x1C << TC_READREQ_ADDR_Pos);
            }
        }
        else
        {
            ulReadReq |= (0x10 << TC_READREQ_ADDR_Pos);
        }
    }

    WriteSync();
    m_pstTC->COUNT32.CTRLA.reg = ulCtrlA;
    WriteSync();
    m_pstTC->COUNT32.CTRLBSET.reg = ulCtrlB;
    WriteSync();
    m_pstTC->COUNT32.CTRLC.reg = ulCtrlC;
    WriteSync();
    m_pstTC->COUNT32.READREQ.reg = ulReadReq;
    WriteSync();
    m_pstTC->COUNT32.EVCTRL.reg = ulEventCtrl;
}
