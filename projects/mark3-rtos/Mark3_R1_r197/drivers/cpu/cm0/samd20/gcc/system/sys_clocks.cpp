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
  \file  sys_clocks.cpp
  \brief Class implementation for Atmel SAM-D20 clocks
 */

#include "sys_clocks.h"

//---------------------------------------------------------------------------
SysClock::SysClock()
{
    m_eClock = CLK_DFLL48;
    m_eClockGen = GCLK_0;
    m_bLock = false;
}

//---------------------------------------------------------------------------
SysClock::SysClock( Clock_t eClock_ )
{
    SysClock();
    if (eClock_ < CLK_COUNT)
    {
        m_eClock = eClock_;
    }
}

//---------------------------------------------------------------------------
ClockError_t SysClock::SetClockID( Clock_t eClock_ )
{
    if (eClock_ >= CLK_COUNT)
    {
        return CLK_ERR_RANGE;
    }
    m_eClock = eClock_;
    return CLK_ERR_EOK;
}

//---------------------------------------------------------------------------
ClockError_t SysClock::SetGenerator( ClockGenerator_t eClockGen_ )
{
    if (eClockGen_ >= GCLK_COUNT)
    {
        return CLK_ERR_RANGE;
    }
    m_eClockGen = eClockGen_;
    return CLK_ERR_EOK;
}

//---------------------------------------------------------------------------
void SysClock::LockOnEnable( bool bLock_ )
{
    m_bLock = bLock_;
}

//---------------------------------------------------------------------------
ClockError_t SysClock::Setup( Clock_t eClock_, ClockGenerator_t eClockGen_, bool bLock_ )
{
    ClockError_t eErr;
    eErr = SetClockID(eClock_);
    if (eErr != CLK_ERR_EOK)
    {
        return eErr;
    }
    eErr = SetGenerator(eClockGen_);
    if (eErr != CLK_ERR_EOK)
    {
        return eErr;
    }
    LockOnEnable(bLock_);
    return CLK_ERR_EOK;
}

//---------------------------------------------------------------------------
void SysClock::CommitConfig( void )
{
    K_ULONG ulConfig = 0;
    ulConfig |= ((K_ULONG)m_eClock) << GCLK_CLKCTRL_ID_Pos;
    ulConfig |= ((K_ULONG)m_eClockGen) << GCLK_CLKCTRL_GEN_Pos;
    if (m_bLock)
    {
        ulConfig |= GCLK_CLKCTRL_WRTLOCK;
    }
    Enable(false);
    GCLK->CLKCTRL.reg = ulConfig;
}

//---------------------------------------------------------------------------
void SysClock::Enable( bool bEnable_ )
{
    WriteChannel();
    if (bEnable_)
    {
        GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
    }
    else
    {
        GCLK->CLKCTRL.reg &= ~GCLK_CLKCTRL_CLKEN;
        while (GCLK->CLKCTRL.reg & GCLK_CLKCTRL_CLKEN) {}
    }
}

//---------------------------------------------------------------------------
void SysClock::WriteChannel()
{
    *((uint8_t*)&GCLK->CLKCTRL.reg) = (uint8_t)m_eClock;
}

//---------------------------------------------------------------------------
SysClockGenerator::SysClockGenerator()
{
    m_eClockGen			= GCLK_0;
    m_eDivMode			= DIV_MODE_LINEAR;
    m_eClockSrc			= GCLK_SRC_OSC8M;
    m_usDivisor			= 0;
    m_bRunInStandby		= true;
    m_bOutputToPin		= false;
    m_bOutputOffValue	= false;
    m_bImproveDutyCycle = true;
}

//---------------------------------------------------------------------------
SysClockGenerator::SysClockGenerator( ClockGenerator_t eClockGen_ )
{
    SysClockGenerator();
    m_eClockGen = eClockGen_;
}

//---------------------------------------------------------------------------
ClockError_t SysClockGenerator::SetGenerator( ClockGenerator_t eGen_ )
{
    if (eGen_ >= GCLK_COUNT)
    {
        return CLK_ERR_RANGE;
    }
    m_eClockGen = eGen_;
    return CLK_ERR_EOK;
}

//---------------------------------------------------------------------------
void SysClockGenerator::SetClockSource( ClockGenSource_t eClockSrc_ )
{
    m_eClockSrc = eClockSrc_;
}

//---------------------------------------------------------------------------
void SysClockGenerator::OutputOffState( bool bState_ )
{
    m_bOutputOffValue = bState_;
}

//---------------------------------------------------------------------------
void SysClockGenerator::PinOutput( bool bOutput_ )
{
    m_bOutputToPin = bOutput_;
}

//---------------------------------------------------------------------------
void SysClockGenerator::RunInStandby( bool bOutput_ )
{
    m_bRunInStandby = bOutput_;
}

//---------------------------------------------------------------------------
void SysClockGenerator::ImproveDutyCycle( bool bImprove_ )
{
    m_bImproveDutyCycle = bImprove_;
}

//---------------------------------------------------------------------------
void SysClockGenerator::SetDivisor( ClockDivideMode_t eMode_, K_USHORT usDivisor_ )
{
    m_eDivMode = eMode_;
    m_usDivisor = usDivisor_;
}

//---------------------------------------------------------------------------
void SysClockGenerator::CommitConfig(void)
{
    K_ULONG ulGenCtrl = 0;
    K_ULONG ulGenDiv = 0;

    ulGenCtrl = m_eClockGen << GCLK_GENCTRL_ID_Pos
              | m_eClockSrc << GCLK_GENCTRL_SRC_Pos;

    if (m_bImproveDutyCycle)
    {
        ulGenCtrl |= GCLK_GENCTRL_IDC;
    }
    if (m_bRunInStandby)
    {
        ulGenCtrl |= GCLK_GENCTRL_RUNSTDBY;
    }
    if (m_eDivMode)
    {
        ulGenCtrl |= GCLK_GENCTRL_DIVSEL;
    }
    if (m_bOutputToPin)
    {
        ulGenCtrl |= GCLK_GENCTRL_OE;
    }
    if (m_bOutputOffValue)
    {
        ulGenCtrl |= GCLK_GENCTRL_OOV;
    }

    ulGenDiv = m_eClockGen << GCLK_GENDIV_ID_Pos
             | m_usDivisor << GCLK_GENDIV_DIV_Pos;

    WriteSync();
    WriteGeneratorDiv();
    WriteSync();

    GCLK->GENDIV.reg = ulGenDiv;

    WriteSync();

    GCLK->GENCTRL.reg = ulGenCtrl | (GCLK->GENCTRL.reg & GCLK_GENCTRL_GENEN);
}

//---------------------------------------------------------------------------
bool SysClockGenerator::IsSyncing()
{
    if (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
void SysClockGenerator::WriteSync()
{
    while (IsSyncing());
}

//---------------------------------------------------------------------------
void SysClockGenerator::WriteGenerator()
{
    *((uint8_t*)&GCLK->GENCTRL.reg) = (uint8_t)m_eClockGen;
}

//---------------------------------------------------------------------------
void SysClockGenerator::WriteGeneratorDiv()
{
    *((uint8_t*)&GCLK->GENDIV.reg) = (uint8_t)m_eClockGen;
}

//---------------------------------------------------------------------------
Source_OSC8M::Source_OSC8M()
{
	m_ePrescalar = OSC8M_PRESCALAR_1;
	m_bOnDemand = true;
	m_bRunStandby = false;
}

//---------------------------------------------------------------------------
void Source_OSC8M::Prescalar(OSC8M_Prescaler_t ePrescalar_)
{
	m_ePrescalar = ePrescalar_;
}

//---------------------------------------------------------------------------
void Source_OSC8M::RunOnDemand(bool bRunOnDemand_)
{
	m_bOnDemand = bRunOnDemand_;
}

//---------------------------------------------------------------------------
void Source_OSC8M::RunInStandby(bool bRunInStandby_)
{
	m_bRunStandby = bRunInStandby_;
}

//---------------------------------------------------------------------------
void Source_OSC8M::CommitConfig()
{
	K_ULONG ulReg;
	ulReg = SYSCTRL->OSC8M.reg;
	ulReg &= ~( SYSCTRL_OSC8M_PRESC_Msk
				| SYSCTRL_OSC8M_ONDEMAND
				| SYSCTRL_OSC8M_RUNSTDBY
			  );

	if (m_ePrescalar)
	{
		ulReg |= (((K_ULONG)m_ePrescalar) << SYSCTRL_OSC8M_PRESC_Pos);
	}
	if (m_bRunStandby)
	{
		ulReg |= SYSCTRL_OSC8M_RUNSTDBY;
	}
	if (m_bOnDemand)
	{
		ulReg |= SYSCTRL_OSC8M_ONDEMAND;
	}

	SYSCTRL->OSC8M.reg = ulReg;
}

//---------------------------------------------------------------------------
void Source_OSC8M::Enable(bool bEnable_)
{
	if (bEnable_)
	{
		SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;
	}
	else
	{
		SYSCTRL->OSC8M.reg &= ~SYSCTRL_OSC8M_ENABLE;
	}
}

//---------------------------------------------------------------------------
Source_XOSC32K::Source_XOSC32K()
{
	m_eStartupTime = XOSC32K_STARTUP_32K;
	m_bEnable32KHz = true;
	m_bEnable1KHz = false;
	m_bAutoAmp = true;
	m_bExternalXtal = true;
	m_bOnDemand = true;
	m_bRunStandby = false;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::StartupTime(XOSC32K_StartupTime_t eTime_)
{
	m_eStartupTime = eTime_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::Enable32KHz(bool bEnable_)
{
	m_bEnable32KHz = bEnable_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::Enable1KHz(bool bEnable_)
{
	m_bEnable1KHz = bEnable_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::GainAmplifier(bool bEnable_)
{
	m_bAutoAmp = bEnable_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::ExternalCrystal(bool bEnable_)
{
	m_bExternalXtal = bEnable_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::RunOnDemand(bool bRunOnDemand_)
{
	m_bOnDemand = bRunOnDemand_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::RunInStandby(bool bRunInStandby_)
{
	m_bRunStandby = bRunInStandby_;
}

//---------------------------------------------------------------------------
void Source_XOSC32K::CommitConfig()
{
	K_ULONG ulReg;
	ulReg = SYSCTRL->XOSC32K.reg;
	ulReg &= ~( SYSCTRL_XOSC32K_STARTUP_Msk
				| SYSCTRL_XOSC32K_EN1K
				| SYSCTRL_XOSC32K_EN32K
				| SYSCTRL_XOSC32K_AAMPEN
				| SYSCTRL_XOSC32K_ENABLE
				| SYSCTRL_XOSC32K_XTALEN
				| SYSCTRL_XOSC32K_ONDEMAND
				| SYSCTRL_XOSC32K_RUNSTDBY
				);

	ulReg |= ((K_ULONG)m_eStartupTime << SYSCTRL_XOSC32K_STARTUP_Pos);

	if (m_bAutoAmp)
	{
		ulReg |= SYSCTRL_XOSC32K_AAMPEN;
	}
	if (m_bEnable1KHz)
	{
		ulReg |= SYSCTRL_XOSC32K_EN1K;
	}
	if (m_bEnable32KHz)
	{
		ulReg |= SYSCTRL_XOSC32K_EN32K;
	}
	if (m_bExternalXtal)
	{
		ulReg |= SYSCTRL_XOSC32K_XTALEN;
	}
	if (m_bOnDemand)
	{
		ulReg |= SYSCTRL_XOSC32K_ONDEMAND;
	}
	if (m_bRunStandby)
	{
		ulReg |= SYSCTRL_XOSC32K_RUNSTDBY;
	}
}

//---------------------------------------------------------------------------
void Source_XOSC32K::Enable(bool bEnable_)
{
	if (bEnable_)
	{
		SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE;
	}
	else
	{
		SYSCTRL->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ENABLE;
	}
}

//---------------------------------------------------------------------------
Source_DFLL48M::Source_DFLL48M()
{
	m_bRunStandby = false;
	m_bOnDemand = true;
	m_bChillCycle = true;
	m_bQuickLock = true;
	m_bLoseLockAfterWake = true;
	m_bClockMode = true;
	
	m_usMultiplier = 6;
	m_ucMaxCoarseStep = 1;
	m_usMaxFineStep = 1;
	
	m_ucCoarseAdjust = (0x1F / 4);
	m_usFineAdjust = (0xFF / 4);
}

//---------------------------------------------------------------------------
void Source_DFLL48M::ClockMode(bool bClosedLoop_)
{
	m_bClockMode = bClosedLoop_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::RunOnDemand(bool bRunOnDemand_)
{
	m_bOnDemand = bRunOnDemand_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::RunInStandby(bool bRunInStandby_)
{
	m_bRunStandby = bRunInStandby_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::ChillCycle(bool bEnable_)
{
	m_bChillCycle = bEnable_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::LoseLockAfterWakeup(bool bEnable_)
{
	m_bLoseLockAfterWake = bEnable_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::QuickLock(bool bEnable_)
{
	m_bQuickLock = bEnable_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::Multiplier(K_USHORT usMultiplier_)
{
	m_usMultiplier = usMultiplier_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::MaxCoarseStep(K_UCHAR ucStep_)
{
	m_ucMaxCoarseStep = ucStep_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::MaxFineStep(K_USHORT usStep_)
{
	m_usMaxFineStep = usStep_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::CoarseAdjust(K_UCHAR ucVal_)
{
	m_ucCoarseAdjust = ucVal_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::FineAdjust(K_USHORT usVal_)
{
	m_usFineAdjust = usVal_;
}

//---------------------------------------------------------------------------
void Source_DFLL48M::CommitConfig()
{
	K_ULONG ulMul = 0;
	K_ULONG ulVal = 0;
	K_ULONG ulCtrl = 0;
	SysClock clClock;
	
	if (m_bClockMode)
	{
		ulCtrl |= SYSCTRL_DFLLCTRL_MODE;
	}
	
	if (!m_bChillCycle)	// In the register,  Disabled = 1
	{
		ulCtrl |= SYSCTRL_DFLLCTRL_CCDIS;
	}
	
	if (!m_bQuickLock) // In the register, Disabled = 1
	{
		ulCtrl |= SYSCTRL_DFLLCTRL_QLDIS;
	}
	
	if (m_bLoseLockAfterWake)
	{
		ulCtrl |= SYSCTRL_DFLLCTRL_LLAW;
	}
	
	if (m_bOnDemand)
	{
		ulCtrl |= SYSCTRL_DFLLCTRL_ONDEMAND;
	}
	
	if (m_bRunStandby)
	{
		ulCtrl |= SYSCTRL_DFLLCTRL_RUNSTDBY;
	}
	
	// Only set multiplier in closed-loop mode.
	if (m_bClockMode)
	{
		ulMul = ( ((K_ULONG)m_usMultiplier) << SYSCTRL_DFLLMUL_MUL_Pos )
		| (((K_ULONG)m_ucMaxCoarseStep) << SYSCTRL_DFLLMUL_CSTEP_Pos)
		| (((K_ULONG)m_usMaxFineStep) << SYSCTRL_DFLLMUL_FSTEP_Pos)
		;
	}
	
	ulVal =   ( ((K_ULONG)m_ucCoarseAdjust) << SYSCTRL_DFLLVAL_COARSE_Pos)
	| ( ((K_ULONG)m_usFineAdjust) << SYSCTRL_DFLLVAL_FINE_Pos)
	;

	SYSCTRL->DFLLCTRL.reg = ulCtrl & ~SYSCTRL_DFLLCTRL_ONDEMAND;

	WaitSync();
	
	SYSCTRL->DFLLMUL.reg = ulMul;
	SYSCTRL->DFLLVAL.reg = ulVal;
	
	SYSCTRL->DFLLCTRL.reg = ulCtrl;
	WaitSync();
}

void Source_DFLL48M::Enable(bool bEnable_)
{
	WaitSync();
	if (bEnable_)
	{
		SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE;
	}
	else
	{
		SYSCTRL->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_ENABLE;
	}
}

void Source_DFLL48M::WaitSync()
{
	while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {}
}