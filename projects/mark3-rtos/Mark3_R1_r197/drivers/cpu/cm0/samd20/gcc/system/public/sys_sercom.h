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
  \file  sys_sercom.h
  \brief Basic definitions for Atmel SAM-D20 SERCOM modules
 */


#ifndef __SYS_SERCOM_H__
#define __SYS_SERCOM_H__

#include <samd20.h>

#include "sys_clocks.h"
#include "sys_port.h"
#include "kerneltypes.h"

//---------------------------------------------------------------------------
#define SERCOM0_PAD0_PORT_MUXC          SYS_PORT_A
#define SERCOM0_PAD1_PORT_MUXC          SYS_PORT_A
#define SERCOM0_PAD2_PORT_MUXC          SYS_PORT_A
#define SERCOM0_PAD3_PORT_MUXC          SYS_PORT_A
#define SERCOM0_PAD0_PIN_MUXC           8
#define SERCOM0_PAD1_PIN_MUXC           9
#define SERCOM0_PAD2_PIN_MUXC           10
#define SERCOM0_PAD3_PIN_MUXC           11

#define SERCOM1_PAD0_PORT_MUXC          SYS_PORT_A
#define SERCOM1_PAD1_PORT_MUXC          SYS_PORT_A
#define SERCOM1_PAD2_PORT_MUXC          SYS_PORT_A
#define SERCOM1_PAD3_PORT_MUXC          SYS_PORT_A
#define SERCOM1_PAD0_PIN_MUXC           16
#define SERCOM1_PAD1_PIN_MUXC           17
#define SERCOM1_PAD2_PIN_MUXC           18
#define SERCOM1_PAD3_PIN_MUXC           19

#define SERCOM2_PAD0_PORT_MUXC          SYS_PORT_A
#define SERCOM2_PAD1_PORT_MUXC          SYS_PORT_A
#define SERCOM2_PAD2_PORT_MUXC          SYS_PORT_A
#define SERCOM2_PAD3_PORT_MUXC          SYS_PORT_A
#define SERCOM2_PAD0_PIN_MUXC           12
#define SERCOM2_PAD1_PIN_MUXC           13
#define SERCOM2_PAD2_PIN_MUXC           14
#define SERCOM2_PAD3_PIN_MUXC           15

#define SERCOM3_PAD0_PORT_MUXC          SYS_PORT_A
#define SERCOM3_PAD1_PORT_MUXC          SYS_PORT_A
#define SERCOM3_PAD2_PORT_MUXC          SYS_PORT_A
#define SERCOM3_PAD3_PORT_MUXC          SYS_PORT_A
#define SERCOM3_PAD0_PIN_MUXC           22
#define SERCOM3_PAD1_PIN_MUXC           23
#define SERCOM3_PAD2_PIN_MUXC           24
#define SERCOM3_PAD3_PIN_MUXC           25

#define SERCOM0_PAD0_PORT_MUXD          SYS_PORT_A
#define SERCOM0_PAD1_PORT_MUXD          SYS_PORT_A
#define SERCOM0_PAD2_PORT_MUXD          SYS_PORT_A
#define SERCOM0_PAD3_PORT_MUXD          SYS_PORT_A
#define SERCOM0_PAD0_PIN_MUXD           4
#define SERCOM0_PAD1_PIN_MUXD           5
#define SERCOM0_PAD2_PIN_MUXD           6
#define SERCOM0_PAD3_PIN_MUXD           7

#define SERCOM1_PAD0_PORT_MUXD          SYS_PORT_A
#define SERCOM1_PAD1_PORT_MUXD          SYS_PORT_A
#define SERCOM1_PAD2_PORT_MUXD          SYS_PORT_A
#define SERCOM1_PAD3_PORT_MUXD          SYS_PORT_A
#define SERCOM1_PAD0_PIN_MUXD           0
#define SERCOM1_PAD1_PIN_MUXD           1
#define SERCOM1_PAD2_PIN_MUXD           30
#define SERCOM1_PAD3_PIN_MUXD           31

#define SERCOM2_PAD0_PORT_MUXD          SYS_PORT_A
#define SERCOM2_PAD1_PORT_MUXD          SYS_PORT_A
#define SERCOM2_PAD2_PORT_MUXD          SYS_PORT_A
#define SERCOM2_PAD3_PORT_MUXD          SYS_PORT_A
#define SERCOM2_PAD0_PIN_MUXD           8
#define SERCOM2_PAD1_PIN_MUXD           9
#define SERCOM2_PAD2_PIN_MUXD           10
#define SERCOM2_PAD3_PIN_MUXD           11

#define SERCOM3_PAD0_PORT_MUXD          SYS_PORT_A
#define SERCOM3_PAD1_PORT_MUXD          SYS_PORT_A
#define SERCOM3_PAD2_PORT_MUXD          SYS_PORT_A
#define SERCOM3_PAD3_PORT_MUXD          SYS_PORT_A
#define SERCOM3_PAD0_PIN_MUXD           16
#define SERCOM3_PAD1_PIN_MUXD           17
#define SERCOM3_PAD2_PIN_MUXD           18      // Or 20
#define SERCOM3_PAD3_PIN_MUXD           19      // Or 21

//---------------------------------------------------------------------------
typedef enum
{
    SERCOM_MUX_C,
    SERCOM_MUX_D,
//----
    SERCOM_MUX_COUNT
} SERCOM_Mux_t;

//---------------------------------------------------------------------------
typedef enum
{
    SERCOM_IF_0,
    SERCOM_IF_1,
    SERCOM_IF_2,
    SERCOM_IF_3,
    // SERCOM_IF_4,
    // SERCOM_IF_5,
//----
    SERCOM_IF_COUNT
} SERCOM_Interface_t;

//---------------------------------------------------------------------------
typedef enum
{
    SERCOM_PAD_0,
    SERCOM_PAD_1,
    SERCOM_PAD_2,
    SERCOM_PAD_3,
//----
    SERCOM_PAD_COUNT
} SERCOM_Pad_t;

//---------------------------------------------------------------------------
typedef struct
{
    PortIdentity_t  ePort;
    K_UCHAR         ucIndex;
} SERCOM_Lookup_t;


//---------------------------------------------------------------------------
extern const SERCOM_Lookup_t g_astLookup[ SERCOM_IF_COUNT * SERCOM_PAD_COUNT * SERCOM_MUX_COUNT ];

#endif //__SYS_SERCOM_H__
