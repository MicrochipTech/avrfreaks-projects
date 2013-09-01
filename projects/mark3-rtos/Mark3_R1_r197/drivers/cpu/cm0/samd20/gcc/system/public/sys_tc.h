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
  \file  sys_tc.h
  \brief Atmel SAM D20 Timer Capture Class declarations
 */

#ifndef __SYS_TC_H__
#define __SYS_TC_H__

#include "sys_clocks.h"

//---------------------------------------------------------------------------
typedef enum
{
    TC_MODE_16BIT,
    TC_MODE_8BIT,
    TC_MODE_32BIT,
//----
    TC_MODE_COUNT
} TCMode_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_8BIT_0,
    TC_8BIT_1,
    TC_8BIT_2,
    TC_8BIT_3,
    TC_8BIT_4,
    TC_8BIT_5,
    TC_8BIT_6,
    TC_8BIT_7,
//----
    TC_8BIT_COUNT
} TC8Bit_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_16BIT_0_1,
    TC_16BIT_2_3,
    TC_16BIT_4_5,
    TC_16BIT_6_7,
//----
    TC_16BIT_COUNT
} TC16Bit_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_32BIT_0_1_2_3,
    TC_32BIT_4_5_6_7,
//----
    TC_32BIT_COUNT
} TC32Bit_t;

//---------------------------------------------------------------------------
typedef union
{
    TC8Bit_t  e8Bit;
    TC16Bit_t e16Bit;
    TC32Bit_t e32Bit;
} TCXBit_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_PRESYNCH_GCLK,
    TC_PRESYNCH_PRESCALAR,
    TC_PRESYNCH_RESYNC,
//----
    TC_PRESYNCH_COUNT
} TCPresynch_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_PRESCALAR_1,
    TC_PRESCALAR_2,
    TC_PRESCALAR_4,
    TC_PRESCALAR_8,
    TC_PRESCALAR_16,
    TC_PRESCALAR_64,
    TC_PRESCALAR_256,
    TC_PRESCALAR_1024,
//----
    TC_PRESCALAR_COUNT
} TCPrescalar_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_WAVEFORM_NORMAL,
    TC_WAVEFORM_MATCH,
    TC_WAVEFORM_NORMAL_PWM,
    TC_WAVEFORM_MATCH_PWM,
//----
    TC_WAVEFORM_COUNT
} TCWaveformMode_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_READSYNC_COUNT,
    TC_READSYNC_CC0,
    TC_READSYNC_CC1,
//----
    TC_READSYNC_COUNTx
} TCReadSync_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_COMMAND_NONE,
    TC_COMMAND_RETRIGGER,
    TC_COMMAND_STOP,
//----
    TC_COMMAND_COUNT
} TCCommand_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_ACTION_OFF,
    TC_ACTION_RETRIGGER,
    TC_ACTION_COUNT,
    TC_ACTION_START,
    TC_ACTION_RESERVED_4,
    TC_ACTION_PPW,
    TC_ACTION_PWP,
    TC_ACTION_RESERVED_7,
//----
    TC_ACTION_COUNTx
} TCAction_t;

//---------------------------------------------------------------------------
typedef enum
{
    TC_INT_MC0,    
	TC_INT_MC1,    
    TC_INT_SYNCRDY,
    TC_INT_ERR,
    TC_INT_OVF,
//----
    TC_INT_COUNT
} TCInt_t;

//---------------------------------------------------------------------------
class SysTC
{
public:
	
    void SetMode( TCMode_t eMode_ )
        { m_eMode = eMode_; }

    void SetInterface( TCXBit_t eBit_ );

    void SetWaveformMode( TCWaveformMode_t eWaveformMode_ )
        { m_eWaveformMode = eWaveformMode_; }

    void SetPrescalar( TCPrescalar_t ePrescalar_ )
        { m_ePrescalar = ePrescalar_; }

    void SetPresynch( TCPresynch_t ePresynch_ )
        { m_ePresync = ePresynch_; }

    void SyncContinuous( bool bSync_ )
        { m_bSyncContinuous = bSync_; }

    void SyncRegister( TCReadSync_t eSyncReg_ )
        { m_eSyncReg = eSyncReg_; }

    void SetOneShot( bool bOneShot_ )
        { m_bOneShot = bOneShot_; }

    void SetDirection( bool bCountDown_ )
        { m_bDirection = bCountDown_; }

    void SetCaptureVal( K_UCHAR ucIndex_, K_ULONG ulCaptureVal_ );

    void SetInvertVal( K_UCHAR ucIndex_, bool bInvert_ )
    {
		m_bInvert0 = bInvert_;	
    }
	void EnableOverflow( bool bOverflow_ )
	{
		m_bOverflowEnable = bOverflow_;
	}

    void EnableCapture( K_UCHAR ucIndex_, bool bEnable_);

    K_ULONG GetCount( void );

    void Start( void );

    void Stop( void );

    void EnableInterrupt( TCInt_t eInt_ );

    void DisableInterrupt( TCInt_t eInt_ );

    void ClearInterrupt( TCInt_t eInt_);

    void SetClockSource( ClockGenerator_t eClockGen_ )
    {
        m_eClockGen = eClockGen_;
    }

private:

    K_UCHAR GetInterfaceIndex();

    Tc *GetInterface();

    void SetupClocks();

    void SetupRegisters();

    void WriteSync()
    {
        while (m_pstTC->COUNT32.STATUS.reg & TC_STATUS_SYNCBUSY)
        {
            /* Do Nothing */
        }
    }

    ClockGenerator_t    m_eClockGen;

    TCMode_t            m_eMode;
    TCXBit_t            m_uTCXBit;
    TCWaveformMode_t    m_eWaveformMode;
    TCPrescalar_t       m_ePrescalar;
    TCPresynch_t        m_ePresync;

    bool                m_bSyncContinuous;
    TCReadSync_t        m_eSyncReg;

    bool                m_bOneShot;
    bool                m_bDirection;

    bool                m_bCapture0;
    bool                m_bInvert0;
    bool                m_bMatchCompare0;    
    bool                m_bOverflowEnable;
    bool                m_bTCEventEnable;
    bool                m_bTCEventInvert;

    TCAction_t          m_eAction;
	Tc					*m_pstTC;
};

#endif
