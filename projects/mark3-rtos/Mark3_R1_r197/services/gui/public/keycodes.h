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
    \file "keycodes.h"
    
    \brief Standard ASCII keyboard codes
*/

#ifndef __KEYCODES_H_
#define __KEYCODES_H_

#include "kerneltypes.h"

typedef enum
{
    KEYCODE_LBUTTON = 0x01,
    KEYCODE_RBUTTON,
    KEYCODE_CANCEL,
    KEYCODE_MBUTTON, 
    KEYCODE_BACK = 0x08,
    KEYCODE_TAB, 
    KEYCODE_CLEAR = 0x0C,
    KEYCODE_RETURN,
    KEYCODE_SHIFT = 0x10, 
    KEYCODE_CONTROL, 
    KEYCODE_MENU, 
    KEYCODE_PAUSE, 
    KEYCODE_CAPITAL, 
    KEYCODE_ESCAPE = 0x1B, 
    KEYCODE_SPACE,
    KEYCODE_PRIOR,
    KEYCODE_NEXT, 
    KEYCODE_END,
    KEYCODE_HOME, 
    KEYCODE_LEFT, 
    KEYCODE_UP,
    KEYCODE_RIGHT,
    KEYCODE_DOWN,
    KEYCODE_SELECT,
    KEYCODE_PRINT,
    KEYCODE_EXECUTE,
    KEYCODE_SNAPSHOT,
    KEYCODE_INSERT,
    KEYCODE_DELETE,
    KEYCODE_HELP = 0x2F,
    KEYCODE_0,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_8,
    KEYCODE_9,
    KEYCODE_A,
    KEYCODE_B,
    KEYCODE_C,
    KEYCODE_D,
    KEYCODE_E,
    KEYCODE_F,
    KEYCODE_G,
    KEYCODE_H,
    KEYCODE_I,
    KEYCODE_J,
    KEYCODE_K,
    KEYCODE_L,
    KEYCODE_M,
    KEYCODE_N,
    KEYCODE_O,
    KEYCODE_P,
    KEYCODE_Q,
    KEYCODE_R,
    KEYCODE_S,
    KEYCODE_T,
    KEYCODE_U,
    KEYCODE_V,
    KEYCODE_W,
    KEYCODE_X,
    KEYCODE_Y,
    KEYCODE_Z,
    KEYCODE_NUMPAD0 = 0x60,
    KEYCODE_NUMPAD1, 
    KEYCODE_NUMPAD2, 
    KEYCODE_NUMPAD3, 
    KEYCODE_NUMPAD4, 
    KEYCODE_NUMPAD5, 
    KEYCODE_NUMPAD6, 
    KEYCODE_NUMPAD7, 
    KEYCODE_NUMPAD8, 
    KEYCODE_NUMPAD9, 
    KEYCODE_SEPARATOR = 0x6C,
    KEYCODE_SUBTRACT, 
    KEYCODE_DECIMAL, 
    KEYCODE_DIVIDE, 
    KEYCODE_F1, 
    KEYCODE_F2, 
    KEYCODE_F3, 
    KEYCODE_F4, 
    KEYCODE_F5, 
    KEYCODE_F6, 
    KEYCODE_F7, 
    KEYCODE_F8,
    KEYCODE_F9,
    KEYCODE_F10,
    KEYCODE_F11,
    KEYCODE_F12,
    KEYCODE_F13,
    KEYCODE_F14,
    KEYCODE_F15,
    KEYCODE_F16,
    KEYCODE_F17,
    KEYCODE_F18,
    KEYCODE_F19,
    KEYCODE_F20,
    KEYCODE_F21,
    KEYCODE_F22,
    KEYCODE_F23,
    KEYCODE_F24,
    KEYCODE_NUMLOCK = 0x90,
    KEYCODE_SCROLL,
    KEYCODE_LSHIFT = 0xA0,
    KEYCODE_RSHIFT,
    KEYCODE_LCONTROL,
    KEYCODE_RCONTROL,
    KEYCODE_LMENU,
    KEYCODE_RMENU,
    KEYCODE_PLAY = 0xFA,
    KEYCODE_ZOOM  
} KEYCODE;

#endif //__KEYCODES_H_
