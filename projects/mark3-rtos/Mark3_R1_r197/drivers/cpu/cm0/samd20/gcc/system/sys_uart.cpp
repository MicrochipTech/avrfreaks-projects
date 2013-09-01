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
  \file  sys_uart.cpp
  \brief Basic UART class implementation for Atmel SAM-D20 SERCOM interfac
 */


#include "sys_uart.h"

//---------------------------------------------------------------------------
#define SYSUART_DEFAULT_BAUD            (57600)
#define SYSUART_DEFAULT_CLOCKFREQ       (8000000)

//---------------------------------------------------------------------------
SysUART::SysUART()
{
    m_eInterface    = SERCOM_IF_0;
    m_ulBaud        = SYSUART_DEFAULT_BAUD;
    m_ucStopBits    = 1;
    m_eMux          = SERCOM_MUX_C;
    m_eTxPad        = SERCOM_PAD_2;
    m_eRxPad        = SERCOM_PAD_3;
    m_bParity       = false;
    m_ulClockFreqHz = SYSUART_DEFAULT_CLOCKFREQ;
    m_eClockGen     = GCLK_1;
}

//---------------------------------------------------------------------------
SysUART::SysUART(SERCOM_Interface_t eInterface_)
{
    SysUART();
    m_eInterface    = eInterface_;
}

//---------------------------------------------------------------------------
void SysUART::Open(void)
{
    // Enable clocks/power-management for port
    SetupClocks();

    // Setup physical connections/muxing
    SetupPort();

    // Setup peripheral registers
    SetupRegisters();
}

//---------------------------------------------------------------------------
bool SysUART::Write(K_CHAR cChar_)
{
    SercomUsart *pstPort = GetPort();

    if(!(pstPort->INTFLAG.reg & SERCOM_USART_INTFLAG_DRE))
    {
        return false;
    }

    WriteSync();

    pstPort->DATA.reg = (K_USHORT)cChar_;

    while (!(pstPort->INTFLAG.reg & SERCOM_USART_INTFLAG_TXC))
    {
        // Wait for data to complete transmission
    }
    return true;
}

//---------------------------------------------------------------------------
void SysUART::SetupClocks(void)
{
    Clock_t eClockID;
    SysClock clClock;

    // Enable the clock in the power-management module...
    PM->APBCMASK.reg |= 1 << (PM_APBCMASK_SERCOM0_Pos + (K_ULONG)m_eInterface);

    // Enable the SERCOM Core Clock for the specified interface
    eClockID = (Clock_t)((K_UCHAR)CLK_SERCOM0_CORE + (K_UCHAR)m_eInterface);

    // Hook the SERCOM Core Clock to the appropriate generator
    clClock.SetClockID(eClockID);
    clClock.SetGenerator(m_eClockGen);
    clClock.CommitConfig();

    clClock.Enable(true);
}

//---------------------------------------------------------------------------
void SysUART::SetupPin(const SERCOM_Lookup_t *pstLUT_)
{
    SysPort *pclPort;
    PinMux_t eMux;

    if (SYS_PORT_A == pstLUT_->ePort)
    {
        pclPort = &PortA;
    }
    else
    {
        pclPort = &PortB;
    }

    if (SERCOM_MUX_C == m_eMux)
    {
        eMux = PIN_MUX_C;
    }
    else
    {
        eMux = PIN_MUX_D;
    }

    // Set the pins as inputs, pulled high.
    pclPort->SetDir(pstLUT_->ucIndex, false);
    pclPort->SetPinConfig(pstLUT_->ucIndex, true, true, true);

    // Set the pinmux values for the port to the right value for the
    // peripheral
    pclPort->SetPortMux(pstLUT_->ucIndex, eMux);
}

//---------------------------------------------------------------------------
K_ULONG SysUART::CalculateBaud(void)
{
    /* Temporary variables  */
    uint64_t ratio = 0;
    uint64_t scale = 0;
    uint64_t baud_calculated = 0;

    /* Calculate the BAUD value */
    ratio = ((16 * (uint64_t)m_ulBaud) << 32) / m_ulClockFreqHz;
    scale = ((uint64_t)1 << 32) - ratio;
    baud_calculated = (65536 * scale) >> 32;

    return baud_calculated;
}

//---------------------------------------------------------------------------
void SysUART::SetupRegisters(void)
{
    SercomUsart *pstUART = GetPort();
    K_ULONG ulCtrlA = 0;
    K_ULONG ulCtrlB = 0;
    K_ULONG ulBaud = 0;

    // Register A fields
    // DORD - Data order: 0 = MSB first, 1 = LSB first
    // CPOL - Clock Polarity (N/A for UART)
    // CMODE - 0 = Async
    // FORM[3:0] Frame Format - 0 = USART, 1 = USART w/Parity
    // RXPO[1:0] Receive Data Pin
    // TXPO Tx Data Pinout
    // IBON Buffer Overflow Notification
    // RUNSTDBY Run in Standby
    // MODE - 0 = USART with external clock, 1 = internal
    // ENABLE - 1 = Peripheral enabled
    // SWRST - Software reset

    // Register B fields
    // RXEN - 1 = receive enable
    // TXEN - 1 = transmit enable
    // PMODE - parity mode (0 = even, 1 = odd)
    // SBMODE - # stop bits 0 = 1b, 1 = 2b
    // CHSIZE[2:0] = 0 = 8b, 1 = 9b, 5 = 5b, 6 = 6b, 7 = 7b

    // Default to RX/TX enabled, user-selected parity & stop bits
    // 8-bit character width.
    ulCtrlB |= SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN;
    if (2 == m_ucStopBits)
    {
        ulCtrlB |= SERCOM_USART_CTRLB_SBMODE;
    }

    if (m_bParity)
    {
        ulCtrlA |= (1 << SERCOM_USART_CTRLA_FORM_Pos);
        ulCtrlB |= SERCOM_USART_CTRLB_PMODE;
    }

    ulCtrlA |= SERCOM_USART_CTRLA_DORD; // LSB first!
    ulCtrlA |= 1 << SERCOM_USART_CTRLA_MODE_Pos; // internal clock
    ulCtrlA |= ((K_ULONG)m_eRxPad << SERCOM_USART_CTRLA_RXPO_Pos);
    if (SERCOM_PAD_2 == m_eTxPad)
    {
        ulCtrlA |= SERCOM_USART_CTRLA_TXPO;
    }

    WriteSync();
    ulBaud = CalculateBaud();
    pstUART->BAUD.reg = ulBaud;
    WriteSync();
    pstUART->CTRLA.reg = ulCtrlA;
    WriteSync();
    pstUART->CTRLB.reg = ulCtrlB;

    WriteSync();
    ulCtrlA |= SERCOM_USART_CTRLA_ENABLE;
    pstUART->CTRLA.reg = ulCtrlA;
}

//---------------------------------------------------------------------------
void SysUART::SetupPort(void)
{
    K_USHORT usBaseIdx;

    // Figure out (based on config) where in the LUT to look for our pin configuration
    usBaseIdx =   ((K_USHORT)m_eMux * (K_USHORT)SERCOM_PAD_COUNT * (K_USHORT)SERCOM_IF_COUNT)
                + ((K_USHORT)m_eInterface * (K_USHORT)SERCOM_PAD_COUNT);

    // Setup TX pin as input
    SetupPin(&g_astLookup[usBaseIdx + (K_USHORT)m_eTxPad]);

    // Setup RX pin as input
    SetupPin(&g_astLookup[usBaseIdx + (K_USHORT)m_eRxPad]);
}

//---------------------------------------------------------------------------
SercomUsart *SysUART::GetPort(void)
{
    SercomUsart *pstPort;
    switch (m_eInterface)
    {
    case SERCOM_IF_0:
        pstPort = (SercomUsart*)SERCOM0;
        break;
    case SERCOM_IF_1:
        pstPort = (SercomUsart*)SERCOM1;
        break;
    case SERCOM_IF_2:
        pstPort = (SercomUsart*)SERCOM2;
        break;
    case SERCOM_IF_3:
        pstPort = (SercomUsart*)SERCOM3;
        break;
    default:
        return (SercomUsart*)SERCOM0;
    }
    return pstPort;
}

//---------------------------------------------------------------------------
void SysUART::WriteSync()
{
    SercomUsart *pstPort = GetPort();
    while (pstPort->STATUS.reg & SERCOM_USART_STATUS_SYNCBUSY)
    {
        /* Do Nothing */
    }
}
