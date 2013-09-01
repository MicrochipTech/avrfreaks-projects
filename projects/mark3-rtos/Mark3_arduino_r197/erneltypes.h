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
    \file kerneltypes.h
    \brief Basic data type primatives used throughout the OS
*/

#include <stdint.h>

#ifndef __KERNELTYPES_H__
#define __KERNELTYPES_H__

#if defined(bool)
    #define K_BOOL            bool    
#else
    #define K_BOOL            uint8_t
#endif
    
#define K_CHAR          char
#define K_UCHAR         uint8_t
#define K_USHORT        uint16_t
#define K_SHORT         int16_t
#define K_ULONG         uint32_t
#define K_LONG          int32_t

#define K_ADDR      uint16_t
#define K_WORD      uint8_t

//---------------------------------------------------------------------------
typedef void (*panic_func_t)( K_USHORT usPanicCode_ );

//---------------------------------------------------------------------------
typedef enum
{
    EVENT_FLAG_ALL,
    EVENT_FLAG_ANY,
    EVENT_FLAG_ALL_CLEAR,
    EVENT_FLAG_ANY_CLEAR,
    EVENT_FLAG_MODES,
    EVENT_FLAG_PENDING_UNBLOCK
} EventFlagOperation_t;


#endif
