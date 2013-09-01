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

    \file   kerneltimer.cpp

    \brief  Kernel Timer Implementation for SAM D20
*/

#include "kerneltypes.h"
#include "kerneltimer.h"
#include "mark3cfg.h"

#include <samd20.h>

#if KERNEL_TIMERS_TICKLESS
//---------------------------------------------------------------------------
#define KERNEL_TIMER_TC         TC0
#define KERNEL_TIMER_GCLK       (0)     //GCLK_0
#define KERNEL_TIMER_CLK        (19)    //CLK_TC_0

#define COUNT_OFFSET            (0x10)
#define CC0_OFFSET              (0x18)
#define CC1_OFFSET              (0x1A)

//---------------------------------------------------------------------------
static bool bEnabled = 1;

//---------------------------------------------------------------------------
static void WriteSync()
{
    while (KERNEL_TIMER_TC->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY)
    {
        /* Do Nothing */
    }
}

//---------------------------------------------------------------------------
static void ReadSync(K_USHORT usOffset_)
{
    KERNEL_TIMER_TC->COUNT16.READREQ.reg = TC_READREQ_RREQ |
                                    (((K_ULONG)usOffset_) << TC_READREQ_ADDR_Pos);

    WriteSync();
}

//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{        
    K_ULONG ulReg;
    K_ULONG ulCtrlA;
    K_ULONG ulCtrlB;
    K_ULONG ulCtrlC;
    K_ULONG ulEventCtrl;
    K_ULONG ulReadReq;

    //--- Enable the clock in the power-management registers ---
    PM->APBCMASK.reg |= 1 << (PM_APBCMASK_TC0_Pos);

    ulReg = (((K_ULONG)KERNEL_TIMER_CLK) << GCLK_CLKCTRL_ID_Pos)
          | (((K_ULONG)KERNEL_TIMER_GCLK) << GCLK_CLKCTRL_GEN_Pos);

    //--- Set the clock ID ---
    *((uint8_t*)&GCLK->CLKCTRL.reg) = (uint8_t)KERNEL_TIMER_CLK;

    //--- Disable clock, set config, Re-enable Clock
    GCLK->CLKCTRL.reg &= ~GCLK_CLKCTRL_CLKEN;
    GCLK->CLKCTRL.reg = ulReg;
    GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;

    //--- Setup Registers ---
    ulCtrlA = (((K_ULONG)1) << TC_CTRLA_WAVEGEN_Pos)        // MATCH
            | (((K_ULONG)0) << TC_CTRLA_MODE_Pos)           // 16-bit timer
            | (((K_ULONG)6) << TC_CTRLA_PRESCALER_Pos)      // 256x prescalar
            | (((K_ULONG)0) << TC_CTRLA_PRESCSYNC_Pos);     // Normal presync

    ulCtrlB = (((K_ULONG)0) << TC_CTRLBSET_ONESHOT_Pos)     // Continuous mode (no 1-shot)
            | (((K_ULONG)0) << TC_CTRLBSET_DIR_Pos);        // Count-up

    ulCtrlC = (((K_ULONG)1) << TC_CTRLC_CPTEN_Pos)          // Enable Capture
            | (((K_ULONG)0) << TC_CTRLC_INVEN_Pos);         // Disable invert

    ulEventCtrl = (((K_ULONG)0) << TC_EVCTRL_MCEO_Pos)
            | (((K_ULONG)1) << TC_EVCTRL_OVFEO_Pos)         // Enable overflow event
            | (((K_ULONG)0) << TC_EVCTRL_TCEI_Pos)
            | (((K_ULONG)0) << TC_EVCTRL_TCINV_Pos);

    ulReadReq = (((K_ULONG)1) << TC_READREQ_RCONT_Pos)
            | (((K_ULONG)0x10) << TC_READREQ_ADDR_Pos);     // Constantly sync the COUNT value

    KERNEL_TIMER_TC->COUNT16.CTRLA.reg = ulCtrlA;
    WriteSync();
    KERNEL_TIMER_TC->COUNT16.CTRLBSET.reg = ulCtrlB;
    WriteSync();
    KERNEL_TIMER_TC->COUNT16.CTRLC.reg = ulCtrlC;
    WriteSync();
    KERNEL_TIMER_TC->COUNT16.READREQ.reg = ulReadReq;
    WriteSync();
    KERNEL_TIMER_TC->COUNT16.EVCTRL.reg = ulEventCtrl;
	
	//--- Enable The Counter ---
	WriteSync();
	KERNEL_TIMER_TC->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;

	KERNEL_TIMER_TC->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
	KERNEL_TIMER_TC->COUNT16.INTENSET.reg = TC_INTENSET_OVF;

	WriteSync();
	KERNEL_TIMER_TC->COUNT16.CC[0].reg = 0xFFFF;

	//--- Enable Interrupts ---
	NVIC_EnableIRQ(TC0_IRQn);

	//--- Stop the counter ---
	
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{   
	WriteSync();
	KERNEL_TIMER_TC->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
    WriteSync();
    KERNEL_TIMER_TC->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
}

//---------------------------------------------------------------------------
K_USHORT KernelTimer::Read(void)
{
    // If we're not continuously reading the count register...
    ReadSync(COUNT_OFFSET);
    return KERNEL_TIMER_TC->COUNT16.COUNT.reg;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SubtractExpiry(K_ULONG ulInterval_)
{
    K_USHORT usVal;

    ReadSync(CC0_OFFSET);
    usVal = KERNEL_TIMER_TC->COUNT16.CC[0].reg;
    usVal -= (K_USHORT)ulInterval_;

    WriteSync();
    KERNEL_TIMER_TC->COUNT16.CC[0].reg = usVal;

    return (K_ULONG)usVal;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::TimeToExpiry(void)
{
    K_USHORT usRead;
    K_USHORT usTop;

    usRead = KernelTimer::Read();

    ReadSync(CC0_OFFSET);
    usTop = KERNEL_TIMER_TC->COUNT16.CC[0].reg;

    if (usRead >= usTop)
    {
        return 0;
    }
    else
    {
        return (K_ULONG)(usTop - usRead);
    }
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::GetOvertime(void)
{
    return KernelTimer::Read();
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SetExpiry(K_ULONG ulInterval_)
{
    K_USHORT usSetInterval;    
    if (ulInterval_ > 65535)
    {
        usSetInterval = 65535;
    } 
    else 
    {
        usSetInterval = (K_USHORT)ulInterval_; 
    }

    WriteSync();
    KERNEL_TIMER_TC->COUNT16.CC[0].reg = usSetInterval;
    return (K_ULONG)usSetInterval;
}

//---------------------------------------------------------------------------
void KernelTimer::ClearExpiry(void)
{
    WriteSync();
    KERNEL_TIMER_TC->COUNT16.CC[0].reg = 0xFFFF;
}

//---------------------------------------------------------------------------
K_UCHAR KernelTimer::DI(void)
{    
    K_UCHAR ucRet = (bEnabled == true);

    KernelTimer::RI(0);
    if (bEnabled == true)
    {
        bEnabled = false;
    }
    return ucRet;
}

//---------------------------------------------------------------------------
void KernelTimer::EI(void)
{
    KernelTimer::RI(1);
}

//---------------------------------------------------------------------------
void KernelTimer::RI(bool bEnable_)
{
    if (bEnable_)
    {
        NVIC_EnableIRQ(TC0_IRQn);
        bEnabled = true;
    }
    else
    {
        NVIC_DisableIRQ(TC0_IRQn);
        bEnabled = false;
    }
}
#else
//---------------------------------------------------------------------------
void KernelTimer::Config(void)
{
	
}

//---------------------------------------------------------------------------
void KernelTimer::Start(void)
{
	SysTick_Config(SYSTEM_FREQ / 1000); // 1KHz fixed clock...
	NVIC_EnableIRQ(SysTick_IRQn);
}

//---------------------------------------------------------------------------
void KernelTimer::Stop(void)
{
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

//---------------------------------------------------------------------------
K_USHORT KernelTimer::Read(void)
{
	// Not implemented in this port
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SubtractExpiry(K_ULONG ulInterval_)
{
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::TimeToExpiry(void)
{
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::GetOvertime(void)
{
	return 0;
}

//---------------------------------------------------------------------------
K_ULONG KernelTimer::SetExpiry(K_ULONG ulInterval_)
{
	return 0;
}

//---------------------------------------------------------------------------
void KernelTimer::ClearExpiry(void)
{
}

//-------------------------------------------------------------------------
K_UCHAR KernelTimer::DI(void)
{
	return 0;
}

//---------------------------------------------------------------------------
void KernelTimer::EI(void)
{
	KernelTimer::RI(0);
}

//---------------------------------------------------------------------------
void KernelTimer::RI(bool bEnable_)
{
}

//---------------------------------------------------------------------------
#endif
