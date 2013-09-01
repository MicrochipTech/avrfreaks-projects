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
  \file sys_port.h
  \brief Classes and type definitions for Atmel SAM-D20 ports/pin-muxing.
*/
#ifndef __SYS_PORT_H__
#define __SYS_PORT_H__

#include "kerneltypes.h"
#include <samd20.h>

//---------------------------------------------------------------------------
typedef enum
{
    SYS_PORT_A,
    SYS_PORT_B,
//----
    SYS_PORT_COUNT
} PortIdentity_t;

//---------------------------------------------------------------------------
typedef enum
{
    PIN_MUX_A,
    PIN_MUX_B,
    PIN_MUX_C,
    PIN_MUX_D,
    PIN_MUX_E,
    PIN_MUX_F,
    PIN_MUX_G,
    PIN_MUX_H,
//----
    PIN_MUX_COUNT
} PinMux_t;

//---------------------------------------------------------------------------
class SysPort
{
public:
    SysPort();

    SysPort(PortIdentity_t ePort_);

    void SetDir(K_UCHAR ucPinIndex_, bool bOutput_);

    void SetOut(K_UCHAR ucPinIndex_, bool bLevel_);

    void ToggleOut(K_UCHAR ucPinIndex_);

    bool GetIn(K_UCHAR ucPinIndex_);

    void SetPinConfig(K_UCHAR ucPinIndex_, bool bPullUp_, bool bInputEnable_, bool bMuxWithPeripheral_);

    void SetPortMux(K_UCHAR ucPinIndex_, PinMux_t eMuxFunction_);

private:
    PortGroup *GetPortPointer();
	
	PortGroup *m_pstPort;
	PortIdentity_t m_ePort;
};

//---------------------------------------------------------------------------
extern SysPort PortA;
extern SysPort PortB;

//---------------------------------------------------------------------------

#endif
