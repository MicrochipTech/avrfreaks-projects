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
  \file  sys_sercom.cpp
  \brief Basic definitions and data for Atmel SAM-D20 SERCOM modules
 */

#include "sys_sercom.h"

//---------------------------------------------------------------------------
const SERCOM_Lookup_t g_astLookup[ SERCOM_IF_COUNT * SERCOM_PAD_COUNT * SERCOM_MUX_COUNT ] =
{
    { SERCOM0_PAD0_PORT_MUXC, SERCOM0_PAD0_PIN_MUXC },
    { SERCOM0_PAD1_PORT_MUXC, SERCOM0_PAD1_PIN_MUXC },
    { SERCOM0_PAD2_PORT_MUXC, SERCOM0_PAD2_PIN_MUXC },
    { SERCOM0_PAD3_PORT_MUXC, SERCOM0_PAD3_PIN_MUXC },

    { SERCOM1_PAD0_PORT_MUXC, SERCOM1_PAD0_PIN_MUXC },
    { SERCOM1_PAD1_PORT_MUXC, SERCOM1_PAD1_PIN_MUXC },
    { SERCOM1_PAD2_PORT_MUXC, SERCOM1_PAD2_PIN_MUXC },
    { SERCOM1_PAD3_PORT_MUXC, SERCOM1_PAD3_PIN_MUXC },

    { SERCOM2_PAD0_PORT_MUXC, SERCOM2_PAD0_PIN_MUXC },
    { SERCOM2_PAD1_PORT_MUXC, SERCOM2_PAD1_PIN_MUXC },
    { SERCOM2_PAD2_PORT_MUXC, SERCOM2_PAD2_PIN_MUXC },
    { SERCOM2_PAD3_PORT_MUXC, SERCOM2_PAD3_PIN_MUXC },

    { SERCOM3_PAD0_PORT_MUXC, SERCOM3_PAD0_PIN_MUXC },
    { SERCOM3_PAD1_PORT_MUXC, SERCOM3_PAD1_PIN_MUXC },
    { SERCOM3_PAD2_PORT_MUXC, SERCOM3_PAD2_PIN_MUXC },
    { SERCOM3_PAD3_PORT_MUXC, SERCOM3_PAD3_PIN_MUXC },

    { SERCOM0_PAD0_PORT_MUXD, SERCOM0_PAD0_PIN_MUXD },
    { SERCOM0_PAD1_PORT_MUXD, SERCOM0_PAD1_PIN_MUXD },
    { SERCOM0_PAD2_PORT_MUXD, SERCOM0_PAD2_PIN_MUXD },
    { SERCOM0_PAD3_PORT_MUXD, SERCOM0_PAD3_PIN_MUXD },

    { SERCOM1_PAD0_PORT_MUXD, SERCOM1_PAD0_PIN_MUXD },
    { SERCOM1_PAD1_PORT_MUXD, SERCOM1_PAD1_PIN_MUXD },
    { SERCOM1_PAD2_PORT_MUXD, SERCOM1_PAD2_PIN_MUXD },
    { SERCOM1_PAD3_PORT_MUXD, SERCOM1_PAD3_PIN_MUXD },

    { SERCOM2_PAD0_PORT_MUXD, SERCOM2_PAD0_PIN_MUXD },
    { SERCOM2_PAD1_PORT_MUXD, SERCOM2_PAD1_PIN_MUXD },
    { SERCOM2_PAD2_PORT_MUXD, SERCOM2_PAD2_PIN_MUXD },
    { SERCOM2_PAD3_PORT_MUXD, SERCOM2_PAD3_PIN_MUXD },

    { SERCOM3_PAD0_PORT_MUXD, SERCOM3_PAD0_PIN_MUXD },
    { SERCOM3_PAD1_PORT_MUXD, SERCOM3_PAD1_PIN_MUXD },
    { SERCOM3_PAD2_PORT_MUXD, SERCOM3_PAD2_PIN_MUXD },
    { SERCOM3_PAD3_PORT_MUXD, SERCOM3_PAD3_PIN_MUXD }
};
